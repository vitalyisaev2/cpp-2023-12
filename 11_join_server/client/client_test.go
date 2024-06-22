package main

import (
	"bufio"
	"errors"
	"fmt"
	"io"
	"log"
	"net"
	"strconv"
	"strings"
	"testing"

	"github.com/stretchr/testify/require"
)

type record struct {
	id    int
	value string
}

func (r *record) String() string {
	return fmt.Sprintf("id=%d, value=%s", r.id, r.value)
}

func (r *record) equalsTo(other *record) bool {
	return r.id == other.id && r.value == other.value
}

func parseRecord(line string) (*record, error) {
	split := strings.Split(line, ",")
	if len(split) != 2 {
		return nil, fmt.Errorf("invalid number of lines")
	}

	id, err := strconv.Atoi(split[0])
	if err != nil {
		return nil, fmt.Errorf("convert '%s' to int", split[1])
	}

	return &record{id: id, value: split[1]}, nil
}

func parseFinalResult(response string) (bool, error) {
	if response == "OK" {
		return true, nil
	}

	if strings.HasPrefix(response, "ERR") {
		return true, errors.New(response)
	}

	return false, nil
}

type client struct {
	endpoint string
}

func (c *client) makeConnection() (*net.TCPConn, error) {
	serverAddr, err := net.ResolveTCPAddr("tcp", c.endpoint)
	if err != nil {
		return nil, fmt.Errorf("resolve TCP address: %v", err)
	}

	conn, err := net.DialTCP("tcp", nil, serverAddr)
	if err != nil {
		return nil, fmt.Errorf("dial TCP: %v", err)
	}

	return conn, nil
}

func (c *client) doInsert(tableName string, rec *record) error {
	conn, err := c.makeConnection()
	if err != nil {
		return fmt.Errorf("make connection: %v", err)
	}

	defer conn.Close()

	request := fmt.Sprintf("INSERT %s %d %s\n", tableName, rec.id, rec.value)

	log.Println("Sending request", request)

	n, err := conn.Write([]byte(request))
	if err != nil {
		return fmt.Errorf("conn write: %w", err)
	}

	log.Println("Request sent", n)

	response, err := io.ReadAll(conn)
	if err != nil {
		return fmt.Errorf("conn read: %w", err)
	}

	log.Println("Received response", string(response))

	_, err = parseFinalResult(string(response))
	return err
}

func (c *client) doSelect(tableName string) ([]*record, error) {
	conn, err := c.makeConnection()
	if err != nil {
		return nil, fmt.Errorf("make connection: %v", err)
	}

	defer conn.Close()

	request := fmt.Sprintf("SELECT %s\n", tableName)

	log.Println("Sending request", request)

	n, err := conn.Write([]byte(request))
	if err != nil {
		return nil, fmt.Errorf("write request: %w", err)
	}

	log.Println("Request sent", n)

	records := []*record{}

	scanner := bufio.NewScanner(conn)
	for scanner.Scan() {
		line := scanner.Text()

		if isFinalMessage, err := parseFinalResult(line); isFinalMessage {
			return records, err
		}

		r, err := parseRecord(line)
		if err != nil {
			return nil, fmt.Errorf("parse record from line '%s'", line)
		}

		records = append(records, r)
	}

	if err := scanner.Err(); err != nil {
		return nil, fmt.Errorf("scanner err: %w", err)
	}

	return records, nil
}

func (c *client) doTruncate(tableName string) error {
	conn, err := c.makeConnection()
	if err != nil {
		return fmt.Errorf("make connection: %v", err)
	}

	defer conn.Close()

	request := fmt.Sprintf("TRUNCATE %s\n", tableName)

	log.Println("Sending request", request)

	n, err := conn.Write([]byte(request))
	if err != nil {
		return fmt.Errorf("write request: %w", err)
	}

	log.Println("Request sent", n)

	response, err := io.ReadAll(conn)
	if err != nil {
		return fmt.Errorf("conn read: %w", err)
	}

	log.Println("Received response", string(response))

	_, err = parseFinalResult(string(response))
	return err
}

func newClient(endpoint string) (*client, error) {
	return &client{endpoint: endpoint}, nil
}

func TestMultilineTable(t *testing.T) {
	cl, err := newClient("localhost:10000")
	require.NoError(t, err)

	// Insert two lines

	require.NoError(t, cl.doInsert("A", &record{id: 0, value: "lean"}))
	require.NoError(t, cl.doInsert("A", &record{id: 1, value: "understand"}))

	// Select should return them back

	recordsActual, err := cl.doSelect("A")
	require.NoError(t, err)
	require.Len(t, recordsActual, 2)

	recordsExpected := []*record{
		{0, "lean"},
		{1, "understand"},
	}

	for i := 0; i < len(recordsExpected); i++ {
		require.True(
			t,
			recordsExpected[i].equalsTo(recordsActual[i]),
			fmt.Errorf("expected: %v, actual: %v", recordsExpected[i], recordsActual[i]))
	}

	// Now truncate the table
	require.NoError(t, cl.doTruncate("A"))

	// Table still exists, but empty
	recordsActual, err = cl.doSelect("A")
	require.NoError(t, err)
	require.Len(t, recordsActual, 0)
}

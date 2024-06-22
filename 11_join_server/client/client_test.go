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
	// id    int
	// value string
	values []any
}

func parseRecord(line string) (*record, error) {
	split := strings.Split(line, ",")

	id, err := strconv.Atoi(split[0])
	if err != nil {
		return nil, fmt.Errorf("convert '%s' to int", split[1])
	}

	values := make([]any, 0, len(split))
	values = append(values, id)

	for _, s := range split[1:] {
		values = append(values, s)
	}

	return &record{values: values}, nil
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

func (c *client) doInsert(tableName string, values []any) error {
	conn, err := c.makeConnection()
	if err != nil {
		return fmt.Errorf("make connection: %v", err)
	}

	defer conn.Close()

	request := fmt.Sprintf("INSERT %s %d %s\n", tableName, values[0], values[1])

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
func (c *client) doIntersect(tableName1, tableName2 string) ([]*record, error) {
	conn, err := c.makeConnection()
	if err != nil {
		return nil, fmt.Errorf("make connection: %v", err)
	}

	defer conn.Close()

	request := fmt.Sprintf("INTERSECT %s %s\n", tableName1, tableName2)

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

func newClient(endpoint string) (*client, error) {
	return &client{endpoint: endpoint}, nil
}

func TestTableBasicMethods(t *testing.T) {
	cl, err := newClient("localhost:10000")
	require.NoError(t, err)

	// Insert two lines

	require.NoError(t, cl.doInsert("T", []any{int(0), "lean"}))
	require.NoError(t, cl.doInsert("T", []any{int(1), "understand"}))

	// Select should return them back

	recordsActual, err := cl.doSelect("T")
	require.NoError(t, err)
	require.Len(t, recordsActual, 2)

	recordsExpected := []*record{
		{values: []any{int(0), "lean"}},
		{values: []any{int(1), "understand"}},
	}

	for i := 0; i < len(recordsExpected); i++ {
		require.Equal(
			t,
			recordsExpected[i].values, recordsActual[i].values,
			fmt.Errorf("expected: %v, actual: %v", recordsExpected[i], recordsActual[i]))
	}

	// Now truncate the table
	require.NoError(t, cl.doTruncate("T"))

	// Table still exists, but empty
	recordsActual, err = cl.doSelect("T")
	require.NoError(t, err)
	require.Len(t, recordsActual, 0)
}

func TestTablesIntersection(t *testing.T) {
	cl, err := newClient("localhost:10000")
	require.NoError(t, err)

	require.NoError(t, cl.doInsert("A", []any{int(0), "lean"}))
	require.NoError(t, cl.doInsert("A", []any{int(1), "sweater"}))
	require.NoError(t, cl.doInsert("A", []any{int(2), "frank"}))
	require.NoError(t, cl.doInsert("A", []any{int(3), "violation"}))
	require.NoError(t, cl.doInsert("A", []any{int(4), "quality"}))
	require.NoError(t, cl.doInsert("A", []any{int(5), "precision"}))

	require.NoError(t, cl.doInsert("B", []any{int(3), "proposal"}))
	require.NoError(t, cl.doInsert("B", []any{int(4), "example"}))
	require.NoError(t, cl.doInsert("B", []any{int(5), "lake"}))
	require.NoError(t, cl.doInsert("B", []any{int(6), "flour"}))
	require.NoError(t, cl.doInsert("B", []any{int(7), "wonder"}))
	require.NoError(t, cl.doInsert("B", []any{int(8), "selection"}))

	// Select should return them back

	recordsActual, err := cl.doIntersect("A", "B")
	require.NoError(t, err)
	require.Len(t, recordsActual, 3)

	recordsExpected := []*record{
		{values: []any{int(3), "violation", "proposal"}},
		{values: []any{int(4), "quality", "example"}},
		{values: []any{int(5), "precision", "lake"}},
	}

	for i := 0; i < len(recordsExpected); i++ {
		require.Equal(
			t,
			recordsExpected[i].values, recordsActual[i].values,
			fmt.Errorf("expected: %v, actual: %v", recordsExpected[i], recordsActual[i]))
	}
}

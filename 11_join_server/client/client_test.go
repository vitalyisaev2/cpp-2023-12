package main

import (
	"bufio"
	"bytes"
	"errors"
	"fmt"
	"io"
	"log"
	"net"
	"sync"
	"testing"

	"github.com/stretchr/testify/require"
)

func extractErrorFromResponse(response []byte) error {
	if bytes.Equal(response, []byte("OK")) {
		return nil
	}

	return errors.New(string(response))
}

type record struct {
	id    int
	value string
}

type client struct {
	conn  *net.TCPConn
	mutex sync.Mutex
}

func (c *client) doInsert(tableName string, rec *record) error {
	c.mutex.Lock()
	defer c.mutex.Unlock()

	request := fmt.Sprintf("INSERT %s %d %s\n", tableName, rec.id, rec.value)

	log.Println("Sending request", request)

	n, err := c.conn.Write([]byte(request))
	if err != nil {
		return fmt.Errorf("conn write: %w", err)
	}

	log.Println("Request sent", n)

	response, err := io.ReadAll(c.conn)
	if err != nil {
		return fmt.Errorf("conn read: %w", err)
	}

	log.Println("Received response", string(response))

	return extractErrorFromResponse(response)
}

func (c *client) doSelect(tableName string) ([]*record, error) {
	c.mutex.Lock()
	defer c.mutex.Unlock()

	request := fmt.Sprintf("SELECT %s\n", tableName)

	log.Println("Sending request", request)

	n, err := c.conn.Write([]byte(request))
	if err != nil {
		fmt.Println("Write failed: ", err)
		return nil, fmt.Errorf("write request: %w", err)
	}

	log.Println("Request sent", n)

	scanner := bufio.NewScanner(c.conn)
	for scanner.Scan() {
		line := scanner.Text()
		log.Println("Received response", line)
	}

	if err := scanner.Err(); err != nil {
		return nil, fmt.Errorf("scanner err: %w", err)
	}

	return nil, nil
}

func (c *client) close() {
	c.conn.Close()
}

func newClient(endpoint string) (*client, error) {
	serverAddr, err := net.ResolveTCPAddr("tcp", endpoint)
	if err != nil {
		return nil, fmt.Errorf("resolve TCP address: %v", err)
	}

	conn, err := net.DialTCP("tcp", nil, serverAddr)
	if err != nil {
		return nil, fmt.Errorf("dial TCP: %v", err)
	}

	return &client{conn: conn}, nil
}

func TestMultilineTable(t *testing.T) {
	cl, err := newClient("localhost:10000")
	require.NoError(t, err)

	defer cl.close()

	require.NoError(t, cl.doInsert("A", &record{id: 0, value: "lean"}))
	require.NoError(t, cl.doInsert("A", &record{id: 1, value: "understand"}))
	cl.doSelect("A")
}

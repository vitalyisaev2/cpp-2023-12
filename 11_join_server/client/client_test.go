package main

import (
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
	if bytes.Equal(response, []byte("OK\n")) {
		return nil
	}

	return errors.New(string(response[:len(response)-2]))
}

type client struct {
	conn  *net.TCPConn
	mutex sync.Mutex
}

func (c *client) insert(tableName string, id int, value string) error {
	c.mutex.Lock()
	defer c.mutex.Unlock()

	request := fmt.Sprintf("INSERT %s %d %s\n", tableName, id, value)

	log.Println("Sending request", request)

	n, err := c.conn.Write([]byte(request))
	if err != nil {
		fmt.Println("Write failed: ", err)
		return fmt.Errorf("write request: %w", err)
	}

	log.Println("Request sent", n)

	response, err := io.ReadAll(c.conn)
	if err != nil {
		return fmt.Errorf("read response: %w", err)
	}

	log.Println("Received response", string(response))

	return extractErrorFromResponse(response)
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

func TestScenario(t *testing.T) {
	cl, err := newClient("localhost:10000")
	require.NoError(t, err)

	defer cl.close()

	cl.insert("A", 0, "lean")
}

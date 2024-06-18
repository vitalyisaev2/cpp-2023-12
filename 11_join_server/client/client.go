package main

import (
	"fmt"
	"io"
	"log"
	"net"
	"os"
)

func check(conn *net.TCPConn) error {
	request := []byte("INSERT A 0 lean\n")

	log.Println("Sending request", string(request))

	n, err := conn.Write(request)
	if err != nil {
		fmt.Println("Write failed: ", err)
		return fmt.Errorf("write request: %w", err)
	}

	log.Println("Request sent", n)

	response, err := io.ReadAll(conn)
	if err != nil {
		return fmt.Errorf("read response: %w", err)
	}

	log.Println("Received response", string(response))

	return nil
}

func main() {
	serverAddr, err := net.ResolveTCPAddr("tcp", "localhost:10000")
	if err != nil {
		fmt.Println("Resolve failed: ", err)
		os.Exit(1)
	}

	conn, err := net.DialTCP("tcp", nil, serverAddr)
	if err != nil {
		fmt.Println("Dial failed: ", err)
		os.Exit(1)
	}

	defer conn.Close()

	if err := check(conn); err != nil {
		fmt.Println("Check failed: ", err)
	}
}

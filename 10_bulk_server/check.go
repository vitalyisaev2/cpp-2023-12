package main

import (
	"bytes"
	"fmt"
	"net"
	"os"
	"sync"
)

const connections = 100

func main() {
	tcpServer, err := net.ResolveTCPAddr("tcp", "localhost:10000")
	if err != nil {
		fmt.Println("Resolve failed: ", err)
		os.Exit(1)
	}

	var wg sync.WaitGroup

	for i := 0; i < connections; i++ {
		wg.Add(1)
		go func(start int) {
			defer wg.Done()

			var request bytes.Buffer

			for j := start * 10; j < (start+1)*10; j++ {
				request.WriteString(fmt.Sprintf("%d\n", j))
			}

			fmt.Println("start", start, request.String())

			conn, err := net.DialTCP("tcp", nil, tcpServer)
			if err != nil {
				fmt.Println("Dial failed: ", err)
				os.Exit(1)
			}

			defer conn.Close()

			if _, err := conn.Write(request.Bytes()); err != nil {
				fmt.Println("Write failed: ", err)
				os.Exit(1)
			}

		}(i)
	}

	wg.Wait()
}


# HTTP Request Structure

1. Request line
2. Headers
3. Blank Line
4. Message Body


Steps to write parser
1. Receive raw request data - `recv` function
2. Split the Request into Lines - split at `\r\n`
3. Parse the request line - extract http method, uri, and version
4. parse the headers - read each header line, splitting into key-value pair
5. handle the message body - read specified number of bytes
6. Handle errors and edge cases
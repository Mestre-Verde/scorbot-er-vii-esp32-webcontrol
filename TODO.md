# TODO — M.E.C.H.7 API server

- [X] Create the logic to send data (programs, variables, parameters) from the PC to the controller, like a file.
- [ ] Allow the network to try to reconnect after failing one time to connect.
- [ ] Solve problems with the receiving logic from the PC to the Controller UART1.
- [ ] Create a webpage in Access Point mode in order to configure the API server.
- [ ] Implement an mDNS service to allow the RoboDK post-processor to automatically discover the M.E.C.H.7 API when the ESP32-S2 is connected to a Wi-Fi network in STA mode.
- [ ] Add the XON XOFF logic on the API
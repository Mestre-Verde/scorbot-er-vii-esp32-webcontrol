## 🔧 Project Code

The source code for the ESP32 controller and the web interface is located in the `src/` and `data/` folders.  
The `include/`, `test/`, and `lib/` folders are not relevant for code functionality.

- [📁 ESP32 Main Code](src/)
- [📁 Web Interface Files](data/)  
  ↳ Contains all files to be uploaded to LittleFS.
- [🗂️ Partition Table for ESP32-S2-DevKitC-1](partitions.csv)

## 📄 Documentation

- [📁 Project Documentation](docs/)
> There is not much detailed information about the code yet because I haven't had much time to do it, maybe one day I will be able to do it

## Notes
- The Web Files to be uploaded to LittleFS have comments,so, in order to consume less space, remove the comments but only in a final version and after making a backup.

## 📜 The reason I choose the licence Apache 2.0
This project is licensed under the Apache License, version 2.0, because I believe in open source that is accessible and fair to both users and contributors.

I chose this license because it allows anyone to use, modify, and distribute the code freely, including for commercial purposes, as long as some minimum conditions that I consider essential are met:

- Author Acknowledgment: Anyone who uses or modifies the project must keep my name in the copyright notices. Let's be fair—I spent an entire semester working on this code. Please, just a mention in another project/modification makes me happy 😊.

- Freedom with responsibility: modifications are fully permitted! But if you create an even better version than the current one, share it 😄. And if you adapt the code for another type of robot, I'd love to collaborate on making the project multi-model.

- Protection against patent abuse: No one can use this code and then try to stop me from using it based on patents. I consider this disrespectful—the spirit here is to collaborate, not restrict.

- Transparency: Anyone redistributing modified versions must retain the license notices and, if applicable, the NOTICE file.

In short: Apache 2.0 allows this project to grow and be useful to other people and schools, but without my work being erased, hidden, or used against me.
In the end, we all win: me, the people who own these robots, the schools with these older robots, and the students—who now have access to a more interactive robot (although not as advanced as collaborative robots) and can learn more about robotics.

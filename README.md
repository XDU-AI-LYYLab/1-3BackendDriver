# 1-3BackendDriver
## 0. Compile
### Install dependency
#### Boost.Asio
```Shell
sudo apt install libasio-dev -y
```

## 1. Configuration File and Setup

### Configuration File Format

The configuration for the software is defined in a TOML file. This file includes settings for the log directory, server port, and program details for each group. An example configuration is shown below:

```toml
# config.toml

# Global settings
[config]
log_path = "./log"  # Directory where logs will be saved
port = 18080        # Port number on which the server will run

# Group specific settings
[[group]]
id = 1
path = "/usr/bin/your_program"
args = ["arg1", "arg2"]

[[group]]
id = 2
path = "/usr/bin/another_program"
args = ["arg1"]
```

### Important Configuration Parameters:

- **log_path**: Specifies the directory where the logs will be stored. If the directory does not exist, it will be created.
- **port**: Defines the port number on which the HTTP server will listen.
- **group**: A collection of settings where each group must have a unique `id`, a `path` to the executable, and a list of `args` which are the arguments passed to the executable.

### Setting Up the Configuration File

1. Create a file named `config.toml`.
2. Populate the file according to the structure shown above, adjusting the `path` and `args` as necessary for your programs.
3. Ensure the file is saved in a location accessible by the software when it starts.

## 2. Running the Software

### Prerequisites

Ensure that you have the necessary permissions to execute the software and that all dependencies (such as Crow and TOML libraries) are properly installed and configured.

### Steps to Run the Software

1. **Start the Server:**
   Open a terminal and navigate to the directory containing the software's executable.

   ```bash
   cd /path/to/software
   ```

2. **Execute the Software:**
   Run the software by specifying the configuration file as a command-line argument. Replace `config.toml` with the path to your configuration file if it's located elsewhere.

   ```bash
   ./your_executable -f config.toml
   ```

3. **Access the Server:**
   Once the server is running, you can access it through a web browser or a tool like `curl` to trigger programs as defined in your configuration. For example, to start the program configured for group 1:

   ```bash
   curl http://localhost:18080/group/1
   ```
   
    Or install httpie

    ```bash
    sudo apt-get install httpie  # Debian/Ubuntu
    ```
   and test

    ```bash
    http http://127.0.0.1:18080/group/1
    ```

   This command sends a request to the server to execute the program associated with group 1. The server response will indicate whether the program started successfully or if an error occurred.

### Monitoring and Logs

Logs for each executed program are stored in the directory specified by `log_path` in the configuration file. These logs include the output and error streams of the programs, helping in troubleshooting and monitoring the execution.

---

Ensure this manual is updated as the software evolves or as new features are added. This will help maintain clarity and usability for end users.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>

// Default values
int SLEEP_TIMEOUT = 300;  // Default inactivity timeout in seconds
int CHECK_INTERVAL = 60;  // Default check interval in seconds
char command_to_execute[256];  // Buffer for the command to execute
int include_process_name = 0;  // Flag to check if -i is provided
char input_event_path[64] = "/dev/input/event3"; // Default event device

typedef struct {
    char *process_name;    
    time_t last_activity_time; 
} ProcessInfo;

// Function to display help text
void display_help(const char *program_name) {
    printf("Usage: %s -p process_name1,process_name2,... [-t timeout] [-c check_interval] [-i] [-e event_device] -s script_command\n", program_name);
    printf("Options:\n");
    printf("  -p process_names: Specify the names of the processes to monitor (comma-separated).\n");
    printf("  -t timeout: Optional timeout in seconds (default is 300 seconds).\n");
    printf("  -c check_interval: Optional check interval in seconds (default is 60 seconds).\n");
    printf("  -s script_command: Mandatory script or command to execute when the timeout threshold is met.\n");
    printf("  -i: Optional flag. If provided, passes the process name as an argument to the script command.\n");
    printf("  -e event_device: Optional input event device path (default is /dev/input/event3).\n");
}

// Function to check if the specified process is running
int is_process_running(const char *process_name) {
    char command[256];
    snprintf(command, sizeof(command), "pgrep %s > /dev/null 2>&1", process_name);
    return (system(command) == 0);
}

// Function to monitor input events and update last activity time
void *monitor_input_events(void *arg) {
    ProcessInfo *process_info = (ProcessInfo *)arg;
    char buffer[256];
    int fd = open(input_event_path, O_RDONLY);
    if (fd == -1) {
        perror("Error opening event device");
        return NULL;
    }

    while (read(fd, buffer, sizeof(buffer)) > 0) {
        process_info->last_activity_time = time(NULL);  // Update the last activity timestamp
    }

    close(fd);
    return NULL;
}

// Thread function to monitor a process and its inactivity
void *monitor_process(void *arg) {
    ProcessInfo *process_info = (ProcessInfo *)arg;

    while (1) {
        if (is_process_running(process_info->process_name)) {
            printf("%s is running. Starting idle monitoring...\n", process_info->process_name);

            // Initialize the last activity timestamp
            process_info->last_activity_time = time(NULL);

            // Create a thread to monitor input events for this process
            pthread_t monitor_thread;
            pthread_create(&monitor_thread, NULL, monitor_input_events, (void *)process_info);

            // Monitor for inactivity while the process is running
            while (is_process_running(process_info->process_name)) {
                time_t current_time = time(NULL);
                int inactive_time = current_time - process_info->last_activity_time;

                if (inactive_time >= SLEEP_TIMEOUT) {
                    char final_command[512];

                    // If -i option is provided, pass process name as an argument
                    if (include_process_name) {
                        snprintf(final_command, sizeof(final_command), "%s %s", command_to_execute, process_info->process_name);
                    } else {
                        snprintf(final_command, sizeof(final_command), "%s", command_to_execute);
                    }

                    printf("No activity for %d seconds on process %s. Executing the command: %s\n", SLEEP_TIMEOUT, process_info->process_name, final_command);
                    system(final_command); // Execute the passed command
                    break; // Exit the loop after executing the command
                }

                sleep(CHECK_INTERVAL);
            }

            // Inactivity timer met or process ended
            printf("%s idle threshold met or is no longer running. Resetting idle counter...\n", process_info->process_name);
            pthread_cancel(monitor_thread);  // Stop monitoring input events
            pthread_join(monitor_thread, NULL); // Ensure the thread has finished
        } else {
            sleep(CHECK_INTERVAL);
        }
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    // Parse command-line arguments for -p (process names), -t (timeout), -c (check interval), -s (script/command), -i (optional flag), -e (event device)
    int opt;
    char *process_names_str = NULL;
    while ((opt = getopt(argc, argv, "p:t:c:s:ie:")) != -1) {
        switch (opt) {
            case 'p':
                process_names_str = optarg; // Set the process names (comma-separated)
                break;
            case 't':
                SLEEP_TIMEOUT = atoi(optarg); // Set the timeout
                break;
            case 'c':
                CHECK_INTERVAL = atoi(optarg); // Set the check interval
                break;
            case 's':
                snprintf(command_to_execute, sizeof(command_to_execute), "%s", optarg); // Set the custom command
                break;
            case 'i':
                include_process_name = 1; // Enable passing process name as argument
                break;
            case 'e':
                snprintf(input_event_path, sizeof(input_event_path), "%s", optarg); // Set custom input device
                break;
            default:
                display_help(argv[0]); // Display help if options are invalid
                exit(EXIT_FAILURE);
        }
    }

    // Check if required parameters are provided
    if (!process_names_str || strlen(command_to_execute) == 0) {
        fprintf(stderr, "Error: -p process_names and -s script_command are required.\n");
        display_help(argv[0]);
        exit(EXIT_FAILURE);
    }

    // Split the process names by commas and count the number of processes
    char *process_name = strtok(process_names_str, ",");
    int process_count = 0;
    ProcessInfo process_info_array[10]; // Assuming a maximum of 10 processes for simplicity

    // Store the process names in the process_info_array
    while (process_name != NULL && process_count < 10) {
        process_info_array[process_count].process_name = strdup(process_name); // Store process name
        process_info_array[process_count].last_activity_time = 0; // Initialize activity time
        process_name = strtok(NULL, ",");
        process_count++;
    }

    // Create threads for each process
    pthread_t threads[process_count];
    for (int i = 0; i < process_count; i++) {
        pthread_create(&threads[i], NULL, monitor_process, (void *)&process_info_array[i]);
    }

    // Wait for all threads to complete
    for (int i = 0; i < process_count; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

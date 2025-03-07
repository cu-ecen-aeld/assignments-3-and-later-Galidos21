#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>

int main(int argc, char *argv[]) {
    // Open syslog with LOG_USER facility
    openlog("writer", LOG_PID, LOG_USER);

    // Check if correct number of arguments is provided
    if (argc != 3) {
        syslog(LOG_ERR, "Error: Two arguments are required - a file path and a text string.");
        fprintf(stderr, "Usage: %s <file_path> <text_string>\n", argv[0]);
        closelog();
        return 1;
    }

    const char *writefile = argv[1];
    const char *writestr = argv[2];

    // Open file for writing
    FILE *file = fopen(writefile, "w");
    if (file == NULL) {
        syslog(LOG_ERR, "Error: Could not open file %s for writing.", writefile);
        perror("Error opening file");
        closelog();
        return 1;
    }

    // Write the string to the file
    if (fprintf(file, "%s", writestr) < 0) {
        syslog(LOG_ERR, "Error: Failed to write to file %s.", writefile);
        perror("Error writing to file");
        fclose(file);
        closelog();
        return 1;
    }

    // Close the file
    fclose(file);

    // Log success message with LOG_DEBUG
    syslog(LOG_DEBUG, "Writing '%s' to '%s'", writestr, writefile);

    // Close syslog
    closelog();

    return 0;
}

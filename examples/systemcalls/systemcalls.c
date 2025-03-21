
#include "unity.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "../../../examples/systemcalls/systemcalls.h"
#include "file_write_commands.h"

/**
* This function should:
*   1) Call the do_system() function in systemcalls.c to perform the system() operation.
*   2) Obtain the value returned from return_string_validation() in file_write_commands.h within
*       the assignment autotest submodule at assignment-autotest/test/assignment3/
*   3) Use unity assertion TEST_ASSERT_EQUAL_STRING_MESSAGE the two strings are equal.  See
*       the [unity assertion reference](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityAssertionsReference.md)
*   4) Test with strncmp fo string which return the full path for $HOME,
*       and compare the value using TEST_ASSERT_EQUAL_INT16_MESSAGE.
*   5) Call the do_exec() function in systemcalls.c to perform fork(), execv() and wait() instead of using the system() command.
*   6) Test for a true and false message on the above commands using TEST_ASSERT_FALSE_MESSAGE(TRUE_MESSAGE).
*   7) Call the do_exec_redirect() function in systemcalls.c to perform the same operation as do_exec(),
*       but redirect ouptut to standard out.
*   8) Obtain the value returned from return_string_validation() in file_write_commands.h within
*       the assignment autotest submodule at assignment-autotest/test/assignment3/
*   9) Use unity assertion TEST_ASSERT_EQUAL_STRING_MESSAGE the two strings are equal.
*
**/
#define REDIRECT_FILE "testfile.txt"

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/

    return true;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];

/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/

    va_end(args);

    return true;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];


/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/

    va_end(args);

    return true;
}

void test_systemcalls()
{

    printf("Running tests at %s : function %s\n",__FILE__,__func__);
    TEST_ASSERT_TRUE_MESSAGE(do_system("echo this is a test > " REDIRECT_FILE ),
            "do_system call should return true running echo command");
    char *test_string = malloc_first_line_of_file(REDIRECT_FILE);
    printf("system() echo this is a test returned %s\n", test_string);
    // Testing implementation with testfile.txt output 
    TEST_ASSERT_EQUAL_STRING_MESSAGE("this is a test", test_string,
            "Did not find \"this is a test\" in output of echo command." 
            " Is your system() function implemented properly?");
    free((void *)test_string);
    
    TEST_ASSERT_TRUE_MESSAGE(do_system("echo \"home is $HOME\" > " REDIRECT_FILE),
             "do_system call should return true running echo command");
    test_string = malloc_first_line_of_file(REDIRECT_FILE);
    int test_value = strncmp(test_string, "home is /", 9);
    printf("system() echo home is $HOME returned: %s\n", test_string);
    // Testing implementation with testfile.txt output 
    TEST_ASSERT_EQUAL_INT16_MESSAGE(test_value, 0,
            "The first 9 chars echoed should be \"home is /\".  The last chars will include "
            "the content of the $HOME variable");
    test_value = strncmp(test_string, "home is $HOME", 9);
    TEST_ASSERT_NOT_EQUAL_INT16_MESSAGE(test_value, 0,
            "The $HOME parameter should be expanded when using system()");
    free((void *)test_string);

}

void test_exec_calls()
{
    printf("Running tests at %s : function %s\n",__FILE__,__func__);
    TEST_ASSERT_FALSE_MESSAGE(do_exec(2, "echo", "Testing execv implementation with echo"),
             "The exec() function should have returned false since echo was not specified"
             " with absolute path as a command and PATH expansion is not performed.");
    TEST_ASSERT_FALSE_MESSAGE(do_exec(3, "/usr/bin/test","-f","echo"),
             "The exec() function should have returned false since echo was not specified"
             " with absolute path in argument to the test executable.");
    TEST_ASSERT_TRUE_MESSAGE(do_exec(3, "/usr/bin/test","-f","/bin/echo"),
             "The function should return true since /bin/echo represents the echo command"
             " and test -f verifies this is a valid file");
}

void test_exec_redirect_calls()
{
    printf("Running tests at %s : function %s\n",__FILE__,__func__);
    do_exec_redirect(REDIRECT_FILE, 3, "/bin/sh", "-c", "echo home is $HOME");
    char *test_string = malloc_first_line_of_file(REDIRECT_FILE);
    TEST_ASSERT_NOT_NULL_MESSAGE(test_string,"Nothing written to file at " REDIRECT_FILE );
    if( test_string != NULL ) {
        int test_value = strncmp(test_string, "home is /", 9);
        printf("execv /bin/sh -c echo home is $HOME returned %s\n", test_string);
        // Testing implementation with testfile.txt output 
        TEST_ASSERT_EQUAL_INT16_MESSAGE(test_value, 0,
                "The first 9 chars echoed should be \"home is /\".  The last chars will include "
                "the content of the $HOME variable");
        test_value = strncmp(test_string, "home is $HOME", 9);
        TEST_ASSERT_NOT_EQUAL_INT16_MESSAGE(test_value, 0,
                "The $HOME parameter should be expanded when using /bin/sh with do_exec()");
        free(test_string);
    }

    do_exec_redirect(REDIRECT_FILE, 2, "/bin/echo", "home is $HOME");
    test_string = malloc_first_line_of_file(REDIRECT_FILE);
    TEST_ASSERT_NOT_NULL_MESSAGE(test_string,"Nothing written to file at " REDIRECT_FILE );
    if( test_string != NULL ) {
        printf("execv /bin/echo home is $HOME returned %s\n", test_string);
        // Testing implementation with testfile.txt output 
        TEST_ASSERT_EQUAL_STRING_MESSAGE("home is $HOME", test_string, 
                    "The variable $HOME should not be expanded using execv()");
        free(test_string);
    }
}

/****************************************************************************
*
*  File: weblogger.c
*  Author: Paul Greenberg (http://www.greenberg.pro)
*  Created: 2008
*  Purpose:  Internet Content Filtering Server 
*  Version: 1.0
*  Copyright: (c) 2014 Paul Greenberg <paul@greenberg.pro>
*
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program. If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <signal.h>

/****************************************************************************
* DEFINITIONS                                                               *
****************************************************************************/

#define APP_NAME        "weblogger"
#define APP_VERSION     "0.1"
#define APP_DESC        "Internet Content Filtering Server"
#define APP_COPYRIGHT   "Copyright: (c) 2014 Paul Greenberg <paul@greenberg.pro>"
#define APP_DISCLAIMER  "THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM."

void error(char *, char *);
void print_app_banner(void);
void print_app_usage(void);
void recMsg(char *);
void sigProc(int);
char *chomp(char *);
char *strdup(const char *);

bool verbose_flag, port_flag, output_flag;

char * func =     "main";
char * wPort =    "52880";
int    wPortNum = 0;
char * wOut =     "web.log";
char * wFmt =     "txt";


/****************************************************************************
* FUNCTIONS                                                                 *
****************************************************************************/

char *strdup(const char *str) {
 /* return value should be freed with 'free()' when no longer needed */
 size_t len;
 char *copy;
 len = strlen(str) + 1; /* room for '\0' */
 copy = malloc(len);
 if (copy != NULL) {
  strcpy(copy, str);
 }
 return copy; /* returns NULL of error */
}

char *chomp(char *str) {
 /* removes the newline character from a string */
 int i;
 for (i = 0; i < (int)strlen(str); i++) {
  if ( str[i] == '\n' || str[i] == '\r' ) {
   str[i] = '\0';
  }
 }
 return str;
}

void error(char *err_msg, char *err_func) {
 printf ("\n%s ::: => %s\n", err_func, err_msg);
 print_app_banner();
 print_app_usage();
 exit(0);
}

void print_app_banner(void) {
 printf("\n");
 printf("%s - %s\n", APP_NAME, APP_DESC);
 printf("%s\n", APP_COPYRIGHT);
 printf("%s\n", APP_DISCLAIMER);
 printf("\n");
 return;
}

void print_app_usage(void) {
 printf("Usage:\n");
 printf("   %s --port [1025-65535] --output [logfile] --format [raw|splunk]--verbose\n", APP_NAME);
 printf("   %s --port 15868 --output web.log --verbose\n", APP_NAME);
 printf("   %s --port 15868 --output web.log --format raw\n", APP_NAME);
 printf("   %s -p 15868 -o web.log -f raw\n", APP_NAME);
 printf("\n");
 return;
}

void recMsg(char * rMsg) {
 time_t timer;
 timer=time(NULL);
 //printf("The current time is %s.\n",chomp(asctime(localtime(&timer))));
 FILE *fp;
 fp=fopen( wOut, "a");
 if(fp == NULL) {
  fprintf(stdout, "Error: unable to write to '%s'\n", wOut);
  exit(8);
 } else {
  fprintf(fp, "%s => %s", chomp(asctime(localtime(&timer))), rMsg);
 }
 fclose(fp);
 return;
}

void sigProc(int sigMsg) {
 /*
  catching SIGINT SIGHUP SIGTERM
  SIGHUP(1)    kill -HUP <pid>
  SIGINT(2)    Ctrl + C
  SIGTERM(15)  kill or killall
  On many Unix systems during shutdown, init issues SIGTERM to all processes
  that are not essential to powering off, waits a few seconds, and then
  issues SIGKILL to forcibly terminate any such processes that remain.
 */

 time_t timer;
 timer=time(NULL);
 FILE *fp;
 fp=fopen( wOut, "a");
 if(fp == NULL) {
  fprintf(stdout, "Error: unable to write to '%s'\n", wOut);
  exit(8);
 } else {
  fprintf(fp, "%s => signal #%d. weblogger service stopped...\n", chomp(asctime(localtime(&timer))), sigMsg);
 }
 fclose(fp);
 exit(0);
}


/****************************************************************************
* MAIN ROUTINE                                                              *
****************************************************************************/

int main(int argc, char **argv) {

 verbose_flag = false;
 port_flag = false;
 output_flag = false;

 int c;
 while (1) {
  static struct option long_options[] = {
   {"verbose", no_argument,       0, 'v'},
   {"port",    required_argument, 0, 'p'},
   {"output",  required_argument, 0, 'o'},
   {"format",  required_argument, 0, 'f'},
   {0, 0, 0, 0}
  };

  int option_index = 0;
  c = getopt_long (argc, argv, "f:o:p:v", long_options, &option_index);

  /* Detect the end of the options. */
  if (c == -1)
   break;

  switch (c) {
   case 0:
    /* If this option set a flag, do nothing else now. */
    if (long_options[option_index].flag != 0)
     break;
    printf ("%s ::: stage 1 => option %s", func, long_options[option_index].name);
    if (optarg)
     printf (" with arg %s", optarg);
     printf ("\n");
     break;

   case 'f':
    /*printf ("%s ::: => option -f|--format with value '%s' is set\n", func, optarg);*/
    if((wFmt=realloc(NULL, ( strlen(optarg) * 4 + 1) ))==NULL) {
     printf("%s ::: => memory reallocation failed for --format\n", func);
     exit(1);
    } else {
     //printf("%s ::: => memory reallocation succeeded for --format\n", func);
     strncpy(wFmt,optarg,strlen(optarg));
    }
    break;

   case 'o':
    //printf ("%s ::: => option -o|--output with value '%s' (size: %u) is set\n", func, optarg, strlen(optarg));
    if((wOut=realloc(NULL, ( strlen(optarg) * 4 + 1) ))==NULL) {
     printf("%s ::: => memory reallocation failed for --output\n", func);
     exit(1);
    } else {
     //printf("%s ::: => memory reallocation succeeded for --output\n", func);
     strncpy(wOut,optarg,strlen(optarg));
     //memcpy(wOut,optarg,strlen(optarg));
     //strcpy(wOut,optarg);
    }
    output_flag = true;
    break;

   case 'p':
    if((wPort=realloc(NULL, (strlen(optarg) * 4 + 1) ))==NULL) {
     printf("%s ::: => memory reallocation failed for --port\n", func);
     exit(1);
    } else {
     //printf("%s ::: => memory reallocation succeeded for --port\n", func);
     strncpy(wPort,optarg,strlen(optarg));
    }
    /* check whether wPort is numeric in range from 32768-65535 */
    sscanf(wPort,"%d",&wPortNum);
    //printf("%s -> %d\n", wPort, wPortNum);
    if (wPortNum < 1025 || wPortNum > 65535 ) {
     error("--port MUST be between 1025 and 65535, exiting", func);
    }
    port_flag = true;
    break;

   case 'v':
    /* printf ("%s ::: => option -v|--verbose is set\n", func, optarg); */
    verbose_flag = true;
    break;

   default:
    error("unrecognized option, exiting", func);
  }
 }

 if (optind < argc) {
  error("unexpected option argument, exiting", func);
 }

 if (port_flag == false || output_flag == false) {
  error("both --port and --output options MUST be defined", func);
 }

 if (verbose_flag) {
  print_app_banner();
  printf ("%s ::: => DEBUG is ON\n", func);
  printf ("%s ::: => option -f|--format with value '%s' is set\n", func, wFmt);
  printf ("%s ::: => option -o|--output with value '%s' is set\n", func, wOut);
  printf ("%s ::: => option -p|--port with value '%d' is set\n", func, wPortNum);
 } else {
  print_app_banner();
  printf ("%s ::: => DEBUG is OFF\n", func);
 }

 recMsg("weblogger service started...\n");

 if (signal(SIGINT, sigProc) == SIG_ERR) {
  printf("%s ::: => cannot handle SIGINT!\n", func);
 }
 if (signal(SIGHUP, sigProc) == SIG_ERR) {
  printf("%s ::: => cannot handle SIGHUP!\n", func);
 }
 if (signal(SIGTERM, sigProc) == SIG_ERR) {
  printf("%s ::: => cannot handle SIGTERM!\n", func);
 }

 for(;;) {

 }


exit(0);
}
//end of main() function


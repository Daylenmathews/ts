/* 
 * Author: Luke Hindman, Daylen Mathews
 * Date: Fri 13 Nov 2020 12:21:37 PM PST
 * Description: Starter code for final project (myps)
 */

#define _GNU_SOURCE
#include <stdio.h>
#include "ProcEntry.h"
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>

#define MAX_PROCS 1024 //maximum number of processes to store

/* comparison function for sorting process */
static int pidSort(const void *a, const void *b) {
    ProcEntry *f = *(ProcEntry **)a;
    ProcEntry *s = *(ProcEntry **)b;
         return f->process_id - s->process_id;
}

/* comparison function for sorting by command name */
static int cmdSort(const void *a, const void *b) {
    ProcEntry *f = *(ProcEntry **)a;
    ProcEntry *s = *(ProcEntry **)b;
        return strcmp(f->comm, s->comm);
}

/* helper to check directory entry is valid Directory, by numeric name */
int is_pid_dir(const struct dirent *entry) {
    if (entry->d_type != DT_DIR) return 0;
        return isdigit(entry->d_name[0]);
}

/* main entry point of myps program */
int main (int argc, char * argv[]) {

    //default settings
    char *path = "/proc";
    int sort_by_pid = 1, sort_by_cmd = 0, show_zombies = 0;

    //parse command lie arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
            path = argv[++i];
        } else if (strcmp(argv[i], "-c") == 0) {
            sort_by_cmd = 1;
            sort_by_pid = 0;
        } else if (strcmp(argv[i], "-z") == 0) {
            show_zombies = 1;
        } else if (strcmp(argv[i], "-p") == 0) {
            sort_by_pid = 1;
            sort_by_cmd = 0;
        } else if (strcmp(argv[i], "-h") == 0) {
            fprintf(stdout, "Usage: ./myps [-d <path>] [-p] [-c] [-z] [-h]\n");
            return 0;
        }
    }

    //open specified directory 
    DIR *dir = opendir(path);
    if (!dir) {
        perror("Failed to open directory");
            return 1;
    }

    struct dirent *entry;
    ProcEntry *procList[MAX_PROCS]; //array of pointers
    int count = 0;

    /* read entries from directory */
    while ((entry = readdir(dir)) != NULL) {
        if (!is_pid_dir(entry)) continue;

        char filepath[512];
             snprintf(filepath, sizeof(filepath), "%s/%s/stat", path, entry->d_name);
        ProcEntry *proc = CreateProcEntryFromFile(filepath);
        if (proc) procList[count++] = proc;
    }
    closedir(dir);

    if (sort_by_cmd) {
            qsort(procList, count, sizeof(ProcEntry *), cmdSort);
        } else {
            qsort(procList, count, sizeof(ProcEntry *), pidSort);
        }

    /* print column headers */
    fprintf(stdout, "%7s %7s %5s %5s %5s %7s %-25s %-20s\n",
            "PID", "PPID", "STATE", "UTIME", "STIME", "THREADS", "CMD", "STAT_FILE");

    /* print each process entry */
    for (int i = 0; i < count; i++) {
        if (show_zombies && procList[i]->state != 'Z') {
            DestroyProcEntry(procList[i]);
            continue;
        }
        PrintProcEntry(procList[i]);
        DestroyProcEntry(procList[i]);
    }

    return 0;

}

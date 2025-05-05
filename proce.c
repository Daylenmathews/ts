/**
 * Author: Daylen Mathews
 * Date: 5/4/2025
 * Purpose: Implenets functions for creating, destroying and displaying ProcEntry structs
 * for myps
 */

#include "ProcEntry.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>

/**
 * CreateProcEntry
 * Initializes a ProcEntry struct
 */
ProcEntry *CreateProcEntry(void) {
    ProcEntry *entry = malloc(sizeof(ProcEntry)); //allocate
    if (!entry) return NULL;     //return NULL if fails

    entry->process_id = 0;
    entry->parent_process_id = 0;
    entry->comm = NULL;             //holds command string
    entry->state = '?';         
    entry->utime = 0;
    entry->stime = 0;
    entry->number_of_threads = 0;
    entry->path = NULL;             //will store file path 
    return entry;
}

/* CreateProcEntryFromFile, reads file, parses values, then populates ProcEntry struct */
ProcEntry *CreateProcEntryFromFile(const char statFile[]) {
    if (!statFile) return NULL;

    FILE *fp = fopen(statFile, "r");
    if (!fp) return NULL;

    ProcEntry *entry = CreateProcEntry();
    if (!entry) {
        fclose(fp);
            return NULL;
    }

    char buffer[1024];
    if (!fgets(buffer, sizeof(buffer), fp)) {
        fclose(fp);
        DestroyProcEntry(entry);
            return NULL;
    }
    fclose(fp);

    // Parse values (PID,(COMM),STATE,PPID,UTIME STIME,NUM_THREADS)
    int pid;
    char *start = strchr(buffer, '(');
    char *end = strrchr(buffer, ')');
    if (!start || !end || start >= end) {
        DestroyProcEntry(entry);
            return NULL;
    }

    // Copy comm field between parentheses (preserve all characters including spaces)
    size_t commLen = end - start + 1;
    char *commField = malloc(commLen + 1);
    if (!commField) {
        DestroyProcEntry(entry);
            return NULL;
    }
    strncpy(commField, start, commLen);
    commField[commLen] = '\0';

    // Null-terminate PID section and extract it
    *start = '\0';
    if (sscanf(buffer, "%d", &pid) != 1) {
        free(commField);
        DestroyProcEntry(entry);
            return NULL;
    }

    // continue parsing remaining fields after the closing parenthesis
    char *after = end + 1;
    char state;
    int ppid;
    unsigned long utime, stime;
    long threads;
    int matched = sscanf(after, " %c %d %*s %*s %*s %*s %*s %*s %*s %*s %*s %lu %lu %*s %*s %*s %*s %ld",
                         &state, &ppid, &utime, &stime, &threads);

    if (matched != 5) {
        free(commField);
        DestroyProcEntry(entry);
             return NULL;
    }

    // assign parsed values to entry
    entry->process_id = pid;
    entry->comm = commField;
    entry->state = state;
    entry->parent_process_id = ppid;
    entry->utime = utime;
    entry->stime = stime;
    entry->number_of_threads = threads;
    entry->path = strdup(statFile);

    if (!entry->path) {
        DestroyProcEntry(entry);
        return NULL;
    }

 return entry;
}

/**
 * DestroyProcEntry
 * Frees memory allocated for ProcEnrtyStruct
 */
void DestroyProcEntry(ProcEntry *entry) {
    if (!entry) return;
    free(entry->comm);
    free(entry->path);
    free(entry);
}

/**
 * PrintProcEntry
 * Prints content of ProcEntry 
 */
void PrintProcEntry(ProcEntry *entry) {
    unsigned long int utime = entry->utime / sysconf(_SC_CLK_TCK);
    unsigned long int stime = entry->stime / sysconf(_SC_CLK_TCK);
    fprintf(stdout, "%7d %7d %5c %5lu %5lu %7ld %-25s %-20s\n",
            entry->process_id,
            entry->parent_process_id,
            entry->state,
            utime,
            stime,
            entry->number_of_threads,
            entry->comm,
            entry->path);
}



/**
 * Author: Daylen Mathews
 * Date: 5/4/2025
 * Purpose: This is for Unit tests for the ProcEntry funcions used in myps
 */
#include<stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ProcEntry.h"

/**
 * Test manually creating a Procentry then to destroy
 * Also check if DestroyProcENtry safely handles freeeing memory
 */
void test_CreateDestroy() {
   ProcEntry *entry = malloc(sizeof(ProcEntry));
   entry->comm = strdup("test");
   entry->path = strdup("/fake/path");
   entry->process_id = 123;
   entry->parent_process_id = 1;
   entry->state = 'S';
   entry->utime = 10;
   entry->stime = 20;
   entry->number_of_threads = 2;
   DestroyProcEntry(entry);
   printf("Create/Destroy Test passed\n");
}

/* Test creating a Proc Entry using CreateProcEntryFromFile() */
void test_CreateFromFile_Destroy() {
   ProcEntry *entry = CreateProcEntryFromFile("test_proc/999/stat");
   if (entry) {
       DestroyProcEntry(entry);
       printf("CreateFromFile/Destroy Test passed\n");
   } else {
       printf("CreateFromFile/Destroy Test failed\n");
   }
}

/* Test creating ProcEntry, printing to stdout, then destroying */
void test_CreateFromFile_Print_Destroy() {
   ProcEntry *entry = CreateProcEntryFromFile("test_proc/999/stat");
   if (entry) {
       PrintProcEntry(entry);
       DestroyProcEntry(entry);
       printf("CreateFromFile/Print/Destroy Test passed\n");
   } else {
       printf("CreateFromFile/Print/Destroy Test failed\n");
   }
}

/* Test CreateProcEntryFromFile() when passed NULL */
void test_CreateFromFile_NULL() {
   ProcEntry *entry = CreateProcEntryFromFile(NULL);
   if (!entry) printf("CreateFromFile NULL Test passed\n");
   else printf("CreateFromFile NULL Test failed\n");
}

/* Test behavior when specified stat file does not exist */
void test_CreateFromFile_DoesNotExist() {
   ProcEntry *entry = CreateProcEntryFromFile("test_proc/doesnotexist/stat");
   if (!entry) printf("CreateFromFile DoesNotExist Test passed\n");
   else printf("CreateFromFile DoesNotExist Test failed\n");
}

/* Test behavior when file exists but invalid in format */
void test_CreateFromFile_InvalidFormat() {
   ProcEntry *entry = CreateProcEntryFromFile("test_proc/invalid/stat");
   if (!entry) printf("CreateFromFile InvalidFormat Test passed\n");
   else printf("CreateFromFile InvalidFormat Test failed\n");
}
/* main driver function to run all test cases */
int main(void) {

   test_CreateDestroy();
   test_CreateFromFile_Destroy();
   test_CreateFromFile_Print_Destroy();
   test_CreateFromFile_NULL();
   test_CreateFromFile_DoesNotExist();
   test_CreateFromFile_InvalidFormat();
   return 0;

}

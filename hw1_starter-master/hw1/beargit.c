#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>

#include "beargit.h"
#include "util.h"

/* Implementation Notes:
 *
 * - Functions return 0 if successful, 1 if there is an error.
 * - All error conditions in the function description need to be implemented
 *   and written to stderr. We catch some additional errors for you in main.c.
 * - Output to stdout needs to be exactly as specified in the function description.
 * - Only edit this file (beargit.c)
 * - You are given the following helper functions:
 *   * fs_mkdir(dirname): create directory <dirname>
 *   * fs_rm(filename): delete file <filename>
 *   * fs_mv(src,dst): move file <src> to <dst>, overwriting <dst> if it exists
 *   * fs_cp(src,dst): copy file <src> to <dst>, overwriting <dst> if it exists
 *   * write_string_to_file(filename,str): write <str> to filename (overwriting contents)
 *   * read_string_from_file(filename,str,size): read a string of at most <size> (incl.
 *     NULL character) from file <filename> and store it into <str>. Note that <str>
 *     needs to be large enough to hold that string.
 *  - You NEED to test your code. The autograder we provide does not contain the
 *    full set of tests that we will run on your code. See "Step 5" in the homework spec.
 */

/* beargit init
 *
 * - Create .beargit directory
 * - Create empty .beargit/.index file
 * - Create .beargit/.prev file containing 0..0 commit id
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_init(void) {
  fs_mkdir(".beargit");

  FILE* findex = fopen(".beargit/.index", "w");
  fclose(findex);

  write_string_to_file(".beargit/.prev", "0000000000000000000000000000000000000000");
  return 0;
}


/* beargit add <filename>
 *
 * - Append filename to list in .beargit/.index if it isn't in there yet
 *
 * Possible errors (to stderr):
 * >> ERROR: File <filename> already added
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_add(const char* filename) {
  FILE* findex = fopen(".beargit/.index", "r");
  FILE *fnewindex = fopen(".beargit/.newindex", "w");

  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    if (strcmp(line, filename) == 0) {
      fprintf(stderr, "ERROR: File %s already added\n", filename);
      fclose(findex);
      fclose(fnewindex);
      fs_rm(".beargit/.newindex");
      return 3;
    }

    fprintf(fnewindex, "%s\n", line);
  }

  fprintf(fnewindex, "%s\n", filename);
  fclose(findex);
  fclose(fnewindex);

  fs_mv(".beargit/.newindex", ".beargit/.index");

  return 0;
}


/* beargit rm <filename>
 *
 * See "Step 2" in the homework 1 spec.
 *
 */

int beargit_rm(const char* filename) {
  /* COMPLETE THE REST */
  FILE* findex = fopen(".beargit/.index", "r");
  FILE *fnewindex = fopen(".beargit/.newindex", "w");

  char line[FILENAME_SIZE];
  int exit = 0;

  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    if (strcmp(line, filename) != 0) {
      fprintf(fnewindex, "%s\n", line);
    }
    else{
      exit = 1;
    }
  }
  if(exit == 0){
      fprintf(stderr, "ERROR: File %s not tracked\n", filename);
      fclose(findex);
      fclose(fnewindex);
      fs_rm(".beargit/.newindex");
      return 1;
  }

  fclose(findex);
  fclose(fnewindex);

  fs_mv(".beargit/.newindex", ".beargit/.index");
  return 0;
}

/* beargit commit -m <msg>
 *
 * See "Step 3" in the homework 1 spec.
 *
 */

const char* go_bears = "GO BEARS!";

int is_commit_msg_ok(const char* msg) {
  /* COMPLETE THE REST */
  int j = 0;
  int i;
  for(i = 0; i < COMMIT_ID_SIZE && msg[i] != '\n'; i++){
    if(msg[i] == go_bears[j]){
      j++;
      if(j == 9){
        return 1;
      }
    }
    else{
      j = 0;
    }
  }

  return 0;
}
int power(int a, int b){
  int sum = 1;
  for(int i = 0; i < b; i++)
    sum *= a;
  return sum;
}

int charToInt(char* commit_id){
  int s = 0;
  int i;
  for(i = 0; i < COMMIT_ID_SIZE; i++){
    if(commit_id[COMMIT_ID_SIZE - 2 - i] == '6'){
      s += 1 * power(4, i);
    }else if(commit_id[COMMIT_ID_SIZE - 2 - i] == '1'){
      s += 2 * power(4, i);;
    }else if(commit_id[COMMIT_ID_SIZE - 2 - i] == 'c'){
      s += 3 * power(4, i);;
    }else if(commit_id[COMMIT_ID_SIZE - 2 - i] == '0'){
      s = s;
    }
  }
  return s;
}

/* intToChar(char* s, int a){
  char start[COMMIT_ID_SIZE];
  start[COMMIT_ID_SIZE - 1] = '\0';
  //strcpy(start, "\0")
  int j;
  int temp;

  for(j = 0; j < COMMIT_ID_SIZE - 1; j++){
    temp = a & 3;

    if(temp == 1){
      strcat("6", start);
    }else if(temp == 2){
      strcat("1", start);
    }else if(temp == 3){
      strcat("c", start);
    }else if(temp == 0){
      strcat("0", start);
    }
    a = a >> 2;
  }
  return start;
}*/

void next_commit_id(char* commit_id) {
  /* COMPLETE THE REST */
  int sum = 0;
  sum = charToInt(commit_id);

  int next = sum + 1;
  int temp;
  char start[COMMIT_ID_SIZE];
  //start = intToChar(next);
  start[COMMIT_ID_SIZE - 1] = '\0';
  int j;
  for(j = 0; j < COMMIT_ID_SIZE - 1; j++){
    temp = next & 3;

    if(temp == 1){
      start[COMMIT_ID_SIZE - 2 -j] = '6';
    }else if(temp == 2){
      start[COMMIT_ID_SIZE - 2 - j] = '1';
    }else if(temp == 3){
      start[COMMIT_ID_SIZE - 2 - j] = 'c';
    }else if(temp == 0){
      start[COMMIT_ID_SIZE - 2 -j] = '0';
    }
    next = next >> 2;
  }

  commit_id = start;
  write_string_to_file(".beargit/.prev", commit_id);

}

int beargit_commit(const char* msg) {
  if (!is_commit_msg_ok(msg)) {
    fprintf(stderr, "ERROR: Message must contain \"%s\"\n", go_bears);
    return 1;
  }

  char commit_id[COMMIT_ID_SIZE];
  read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);
  next_commit_id(commit_id);

  /* COMPLETE THE REST */
  read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);
  char buffer[FILENAME_SIZE];
  int n = sprintf(buffer, ".beargit/%s", commit_id);
  fs_mkdir(buffer);

  char bufferr[FILENAME_SIZE];
  n = sprintf(bufferr, ".beargit/%s/.index", commit_id);
  fs_cp(".beargit/.index", bufferr);

  char buffera[FILENAME_SIZE];
  int x = sprintf(buffera, ".beargit/%s/.prev", commit_id);
  fs_cp(".beargit/.prev", buffera);

  char bufferb[FILENAME_SIZE];
  int m = sprintf(bufferb, ".beargit/%s/.msg", commit_id);
  write_string_to_file(bufferb, msg);
  //read_string_from_file(bufferb, bufferr, 10000);

  return 0;
}

/* beargit status
 *
 * See "Step 1" in the homework 1 spec.
 *
 */

int beargit_status() {
  /* COMPLETE THE REST */
  FILE* findex = fopen(".beargit/.index", "r");
  if(findex == NULL)
    return 0;
  char *s;
  char reading[FILENAME_SIZE];
  int count = 0;
  s = fgets(reading, FILENAME_SIZE, findex);
  printf("Tracked files: \n\n");
  if(s == NULL){
    fprintf(stderr, "ERROR No files tracked\n");
    return 1;
  }

  while(s != NULL){
    count++;
    printf("%s\n", reading);
    s = fgets(reading, FILENAME_SIZE, findex);
  }
  fclose(findex);
  free(s);
  printf("%d files total\n", count);
}

/* beargit log
 *
 * See "Step 4" in the homework 1 spec.
 *
 */

int beargit_log() {
  /* COMPLETE THE REST */
  char lastId[COMMIT_ID_SIZE];
  FILE * prev = fopen(".beargit/.prev", "r");
  char* s = fgets(lastId, COMMIT_ID_SIZE, prev);
  int lastOne = charToInt(lastId);

  if(lastOne == 0){
    fprintf(stderr, "ERROR: There are no commits");
    return 1;
  }
  int i;
  int n;
  char start[COMMIT_ID_SIZE];
  start[COMMIT_ID_SIZE - 1] = '\0';
  char msg[MSG_SIZE];
  char bufferMSG[COMMIT_ID_SIZE + sizeof(".beargit/.") + sizeof("/.msg")];

  for(i = 1; i <= lastOne; i++){
    int j;
    int temp = i;
    for(j = 0; j < COMMIT_ID_SIZE - 1; j++){

      int temp1 = temp & 3;

      if(temp1 == 1){
        start[COMMIT_ID_SIZE - 2 -j] = '6';
      }else if(temp1 == 2){
        start[COMMIT_ID_SIZE - 2 - j] = '1';
      }else if(temp1 == 3){
        start[COMMIT_ID_SIZE - 2 - j] = 'c';
      }else if(temp1 == 0){
        start[COMMIT_ID_SIZE - 2 -j] = '0';
      }
      temp = temp >> 2;
    }

    n = sprintf(bufferMSG, ".beargit/%s/.msg", start);
    read_string_from_file(bufferMSG, msg, MSG_SIZE);
    fprintf(stdout, "\ncommit %s\n\t%s\n\n", start, msg);

  }

  return 0;
}

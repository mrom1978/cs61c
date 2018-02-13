#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>

#include "beargit.h"
#include "util.h"
const char* digits = "61c";
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

  FILE* fbranches = fopen(".beargit/.branches", "w");
  fprintf(fbranches, "%s\n", "master");
  fclose(fbranches);

  write_string_to_file(".beargit/.prev", "0000000000000000000000000000000000000000");
  write_string_to_file(".beargit/.current_branch", "master");

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
  //FILE *curren = fopen(".beargit/.branch_master/.index", "w");

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
    //fprintf(curren, "%s\n", line);
  }

  fprintf(fnewindex, "%s\n", filename);
  fclose(findex);
  fclose(fnewindex);
  //fclose(curren);

  fs_mv(".beargit/.newindex", ".beargit/.index");

  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);

  char current_branch_index[BRANCHNAME_SIZE + sizeof(".beargit/.index_")];
  int n = sprintf(current_branch_index, ".beargit/.index_%s", current_branch);
  fs_cp(".beargit/.index", current_branch_index);

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
  int length = strlen(commit_id);
  for(i = 0; i < length; i++){
    if(commit_id[length - 1 - i] == '6'){
      s = 1 * power(4, i);
    }else if(commit_id[length - 1 - i] == '1'){
      s += 2 * power(4, i);
    }else if(commit_id[length - 1 - i] == 'c'){
      s += 3 * power(4, i);
    }else if(commit_id[length - 1 - i] == '0'){
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

void next_commit_id_hw1(char* commit_id) {
  /* COMPLETE THE REST */
  //printf("fourth %s\n", commit_id);
  //printf("fifth %s\n", commit_id - 10);
  int sum = 0;
  sum = charToInt(commit_id);
  //printf("sum is %d\n", sum);
  int next = sum + 1;
  int temp;
  //int length = 30;
  //char start[COMMIT_ID_SIZE];
  //start = intToChar(next);
  commit_id[30] = '\0';
  int j;
  for(j = 0; j < 30; j++){
    temp = next & 3;
    //printf("pin %d\n", temp);
    if(temp == 1){
      commit_id[29 - j] = '6';
    }else if(temp == 2){
      commit_id[29 - j] = '1';
    }else if(temp == 3){
      commit_id[29 - j] = 'c';
    }else if(temp == 0){
      commit_id[COMMIT_ID_SIZE - 2 - j - BRANCHNAME_SIZE] = '0';
    }
    next = next >> 2;
  }

  //("sixth %s\n", commit_id);
  //read_string_from_file(".beargit/.prev", start, COMMIT_ID_SIZE);
  //printf("yomon %s\n", start);

}

int beargit_commit_hw1(const char* msg) {

  if (!is_commit_msg_ok(msg)) {
    fprintf(stderr, "ERROR: Message must contain \"%s\"\n", go_bears);
    return 1;
  }
  //int length = strlen(msg);
  char commit_id[COMMIT_ID_SIZE];
  char previd[COMMIT_ID_SIZE];
  read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);
  read_string_from_file(".beargit/.prev", previd, COMMIT_ID_SIZE);

  next_commit_id(commit_id);

  /* COMPLETE THE REST */
  //commit_id = commit_id - 10;
  //printf("mm %s\n", commit_id);
  //read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);
  char buffer[FILENAME_SIZE];
  int n = sprintf(buffer, ".beargit/%s", commit_id);
  fs_mkdir(buffer);

  char bufferr[FILENAME_SIZE];
  n = sprintf(bufferr, ".beargit/%s/.index", commit_id);
  fs_cp(".beargit/.index", bufferr);

  char buffera[FILENAME_SIZE];
  int x = sprintf(buffera, ".beargit/%s/.prev", commit_id);
  fs_cp(".beargit/.prev", buffera);
  FILE* index = fopen(".beargit/.index", "r");
  char bufferb[FILENAME_SIZE];

  while(fgets(bufferb, sizeof(bufferb), index)){
    strtok(bufferb, "\n");
    //printf("what is this %s\n", bufferb);
    char current_file_dir[COMMIT_ID_SIZE +25];
    sprintf(current_file_dir, ".beargit/%s/%s", commit_id, bufferb);
    fs_cp(bufferb, current_file_dir);
  }
  char mes[MSG_SIZE];
  int m = sprintf(mes, ".beargit/%s/.msg", commit_id);
  write_string_to_file(mes, msg);
  fclose(index);
  //read_string_from_file(bufferb, bufferr, 10000);
  write_string_to_file(".beargit/.prev", commit_id);
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
  if(strlen(reading) == 0){
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
  int n;
  char lastId[COMMIT_ID_SIZE];
  read_string_from_file(".beargit/.prev", lastId, COMMIT_ID_SIZE);

  char current_commit_index[COMMIT_ID_SIZE + sizeof(".beargit/.index")];
  n = sprintf(current_commit_index, ".beargit/%s", lastId);
  //printf("hay %s & %s\n" , current_commit_index, lastId);
  if(!fopen(current_commit_index, "r") || !strcmp(lastId, "0000000000000000000000000000000000000000")){
    fprintf(stderr, "ERROR: There are no commits");
    return 1;
  }
  //printf("yo %s\n", lastId);
  char msg[MSG_SIZE];
  char bufferMSG[COMMIT_ID_SIZE + sizeof(".beargit/.") + sizeof("/.msg")];

  char *s;

  do{
    strcat(current_commit_index, "/.msg");
    read_string_from_file(current_commit_index, msg, MSG_SIZE);
    fprintf(stdout, "\ncommit %s\n\t%s\n\n", lastId, msg);
    sprintf(bufferMSG, ".beargit/%s/.prev", lastId);
    read_string_from_file(bufferMSG, lastId, COMMIT_ID_SIZE);
    sprintf(current_commit_index, ".beargit/%s", lastId);
  }while(fopen(current_commit_index, "r") );

  return 0;
}

// ---------------------------------------
// HOMEWORK 2
// ---------------------------------------

// This adds a check to beargit_commit that ensures we are on the HEAD of a branch.
int beargit_commit(const char* msg) {
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);

  if (strlen(current_branch) == 0) {
    fprintf(stderr, "ERROR: Need to be on HEAD of a branch to commit\n");
    return 1;
  }

  return beargit_commit_hw1(msg);
}



void next_commit_id(char* commit_id) {
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);

  // The first COMMIT_ID_BRANCH_BYTES=10 characters of the commit ID will
  // be used to encode the current branch number. This is necessary to avoid
  // duplicate IDs in different branches, as they can have the same pre-
  // decessor (so next_commit_id has to depend on something else).
  int n = get_branch_number(current_branch);
  for (int i = 0; i < COMMIT_ID_BRANCH_BYTES; i++) {
    // This translates the branch number into base 3 and substitutes 0 for
    // 6, 1 for 1 and c for 2.
    commit_id[i] = digits[n%3];
    n /= 3;
  }

  //printf("this is branch %s\n", commit_id);
  //printf("seocnd %s\n", commit_id + 10);
  //printf("thris %s\n", commit_id);
  // Use next_commit_id to fill in the rest of the commit ID.
  next_commit_id_hw1(commit_id + COMMIT_ID_BRANCH_BYTES);
}


// This helper function returns the branch number for a specific branch, or
// returns -1 if the branch does not exist.
int get_branch_number(const char* branch_name) {
  FILE* fbranches = fopen(".beargit/.branches", "r");

  int branch_index = -1;
  int counter = 0;
  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), fbranches)) {
    strtok(line, "\n");
    if (strcmp(line, branch_name) == 0) {
      branch_index = counter;
    }
    counter++;
  }

  fclose(fbranches);

  return branch_index;
}

/* beargit branch
 *
 * See "Step 6" in the homework 1 spec.
 *
 */

int beargit_branch() {
  /* COMPLETE THE REST */
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);
  int n = get_branch_number(current_branch);

  FILE* fbranches = fopen(".beargit/.branches", "r");
  char line[FILENAME_SIZE];

  while(fgets(line, sizeof(line), fbranches)){
    strtok(line, "\n");
    if(strcmp(line, current_branch) == 0){
      fprintf(stdout, "* <%s>\n", line);
    }else{
      fprintf(stdout, "  <%s>\n", line);
    }
  }
  return 0;
}

/* beargit checkout
 *
 * See "Step 7" in the homework 1 spec.
 *
 */

int checkout_commit(const char* commit_id) {
  /* COMPLETE THE REST */
  FILE* findex = fopen(".beargit/.index", "w");
  if(findex == NULL)
    return 0;
  char *s;
  char reading[FILENAME_SIZE];
  s = fgets(reading, FILENAME_SIZE, findex);

  char current_index[COMMIT_ID_SIZE + sizeof(".beargit/.index")];
  char c[COMMIT_ID_SIZE];
  strcpy(c, commit_id);
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);
  int n = get_branch_number(current_branch);
  for (int i = 0; i < COMMIT_ID_BRANCH_BYTES; i++) {
    // This translates the branch number into base 3 and substitutes 0 for
    // 6, 1 for 1 and c for 2.
    c[i] = digits[n%3];
    n /= 3;
  }

  sprintf(current_index, ".beargit/%s/.index", c);
  //read_string_from_file(current_index, current_index, 100);
  //printf("chekc %s\n", current_index);
  if(findex = fopen(current_index, "r")){
    //fs_cp(current_index, ".beargit/.index");
    s = fgets(reading, FILENAME_SIZE, findex);
    while(s != NULL){
      strtok(reading, "\n");
      beargit_add(reading);
      s = fgets(reading, FILENAME_SIZE, findex);
    }
  }
  write_string_to_file(".beargit/.prev", c);

  return 1;
}

int is_it_a_commit_id(const char* commit_id) {

  if(strlen(commit_id) < COMMIT_ID_SIZE){
    return 0;
  }
  for(int i = 0; i < COMMIT_ID_SIZE - 1; i++){
    if(commit_id[i] != '6' || commit_id[i] != '1' || commit_id[i] != 'c' || commit_id[i] != '0'){
      return 0;
    }
  }
  return 1;
}

int beargit_checkout(const char* arg, int new_branch) {
  // Get the current branch

  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);//mistake 1 - remove double quotation marks from current_branch

  // If not detached, update the current branch by storing the current HEAD into that branch's file...
  // Even if we cancel later, this is still ok.
  if (strlen(current_branch)) {
    char current_branch_file[BRANCHNAME_SIZE+50];
    sprintf(current_branch_file, ".beargit/.branch_%s", current_branch);
    fs_cp(".beargit/.prev", current_branch_file);
  }

  // Check whether the argument is a commit ID. If yes, we just stay in detached mode
  // without actually having to change into any other branch.
  if (is_it_a_commit_id(arg)) {
    char commit_dir[FILENAME_SIZE] = ".beargit/";
    strcat(commit_dir, arg);
    if (!fs_check_dir_exists(commit_dir)) {
      fprintf(stderr, "ERROR: Commit %s does not exist\n", arg);
      return 1;
    }

    // Set the current branch to none (i.e., detached).
    write_string_to_file(".beargit/.current_branch", "");

    return checkout_commit(arg);
  }

  // Just a better name, since we now know the argument is a branch name.
  char branch_name[BRANCHNAME_SIZE];
  strncpy(branch_name, arg, BRANCHNAME_SIZE);

  // Read branches file (giving us the HEAD commit id for that branch).
  int branch_exists = (get_branch_number(branch_name) >= 0);

  // Check for errors.
  if (!(!branch_exists || !new_branch)) {
    fprintf(stderr, "ERROR: A branch named %s already exists\n", branch_name);
    return 1;
  } else if (!branch_exists && !new_branch) {//mistake 2 since new branch is true must be false
    fprintf(stderr, "ERROR: No branch %s exists\n", branch_name);
    return 1;
  }

  // File for the branch we are changing into.
  char branch_file[BRANCHNAME_SIZE] = ".beargit/.branch_";//mistake 3 instead of pointer change to array
  strcat(branch_file, branch_name);

  // Update the branch file if new branch is created (now it can't go wrong anymore)
  if (new_branch) {
    FILE* fbranches = fopen(".beargit/.branches", "a");
    fprintf(fbranches, "%s\n", branch_name);
    fclose(fbranches);
    fs_cp(".beargit/.prev", branch_file);
  }

  write_string_to_file(".beargit/.current_branch", branch_name);

  // Read the head commit ID of this branch.
  char branch_head_commit_id[COMMIT_ID_SIZE];
  read_string_from_file(branch_file, branch_head_commit_id, COMMIT_ID_SIZE);

  // Check out the actual commit.
  return checkout_commit(branch_head_commit_id);
}

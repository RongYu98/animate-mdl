#ifndef MISC_HEADERS_H
#define MISC_HEADERS_H

int num_frames;
char name[128];

struct vary_node {
  
  char name[128]; //string
  double value;
  struct vary_node *next;
};

void my_main();
void print_knobs(); // prints knobs from symbol table
void process_knobs();
void first_pass(); //write

struct vary_node ** second_pass(); //write. Should have an array of linked list, each index is a frame for each knob
// in pass two, just work with vary


#endif

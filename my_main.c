/*========== my_main.c ==========

  This is the only file you need to modify in order
  to get a working mdl project (for now).

  my_main.c will serve as the interpreter for mdl.
  When an mdl script goes through a lexer and parser, 
  the resulting operations will be in the array op[].

  Your job is to go through each entry in op and perform
  the required action from the list below:

  frames: set num_frames (in misc_headers.h) for animation

  basename: set name (in misc_headers.h) for animation

  vary: manipluate knob values between two given frames
        over a specified interval

  set: set a knob to a given value
  
  setknobs: set all knobs to a given value

  push: push a new origin matrix onto the origin stack
  
  pop: remove the top matrix on the origin stack

  move/scale/rotate: create a transformation matrix 
                     based on the provided values, then 
		     multiply the current top of the
		     origins stack by it.

  box/sphere/torus: create a solid object based on the
                    provided values. Store that in a 
		    temporary matrix, multiply it by the
		    current top of the origins stack, then
		    call draw_polygons.

  line: create a line based on the provided values. Store 
        that in a temporary matrix, multiply it by the
	current top of the origins stack, then call draw_lines.

  save: call save_extension with the provided filename

  display: view the image live
  
  jdyrlandweaver
  =========================*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "parser.h"
#include "symtab.h"
#include "y.tab.h"

#include "misc_headers.h"
#include "matrix.h"
#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "stack.h"

/*======== void first_pass()) ==========
  Inputs:   
  Returns: 

  Checks the op array for any animation commands
  (frames, basename, vary)
  
  Should set num_frames and basename if the frames 
  or basename commands are present

  If vary is found, but frames is not, the entire
  program should exit.

  If frames is found, but basename is not, set name
  to some default value, and print out a message
  with the name being used.

  jdyrlandweaver
  ====================*/
void first_pass() {

  num_frames = 1;
  int i;
  int basenameSet = 0;
  int framesSet = 0;
  for (i=0;i<lastop;i++) {
    
    switch (op[i].opcode) {
      
    case FRAMES:
      if (num_frames != 1){
	printf("There's more than one set frame \n");
	exit(0);
      }
      num_frames = op[i].op.frames.num_frames;
      printf("Frames set to: %d \n", num_frames);
      framesSet = 1;
      /*	
	if ( basenameSet == 0){
	printf("No name set, will use 'pic' as the basename\n");
	char *pic = 'pic';
	strcpy( name, pic);
      }
      */
      break;
      
    case BASENAME:
      strncpy( name, op[i].op.basename.p->name, strlen(op[i].op.basename.p->name));
      //name[strlen(op[i].op.basename.p)] = NULL;
      printf("Base Name has been set to: %s \n", name );
      basenameSet = 1;
      break;
    
    case VARY:
      if (framesSet == 0){
	printf("There is no frame number set, quitting \n");
	exit(0);
      }
      break;
    
    }
  
  }
  if ( basenameSet ==  0 && num_frames != 1){
    printf("No name set, will use 'pic' as the basename\n");
    char *pic = 'pic';
    strcpy( name, pic);
  }
}

/*======== struct vary_node ** second_pass()) ==========
  Inputs:   
  Returns: An array of vary_node linked lists

  In order to set the knobs for animation, we need to keep
  a seaprate value for each knob for each frame. We can do
  this by using an array of linked lists. Each array index
  will correspond to a frame (eg. knobs[0] would be the first
  frame, knobs[2] would be the 3rd frame and so on).

  Each index should contain a linked list of vary_nodes, each
  node contains a knob name, a value, and a pointer to the
  next node.

  Go through the opcode array, and when you find vary, go 
  from knobs[0] to knobs[frames-1] and add (or modify) the
  vary_node corresponding to the given knob with the
  appropirate value. 

  05/17/12 09:29:31
  jdyrlandweaver
  ====================*/
struct vary_node ** second_pass() {
  if ( num_frames != 1 ){
    int start, end, startVal, endVal;
    struct vary_node ** values = (struct vary_node **)malloc(num_frames * sizeof(struct vary_node));

    int i;
    for (i=0;i<lastop;i++) {
      
      switch (op[i].opcode) {

      case VARY:
	printf("VARY found\n");
	int a;
	
	//op[lastop].op.vary.p = add_symbol($2,SYM_STRING,0);
	start = op[i].op.vary.start_frame;
	end = op[i].op.vary.end_frame;
	startVal = op[i].op.vary.start_val;
	endVal = op[i].op.vary.end_val;

	//printf("endVal is %d, start is %d end is %d, start is %d\n", endVal, startVal, start, end);        
	//printf("GUIASHODDJHAO\n");
	//printf("%f\n", 1/(end-start)*(a-start));
	for (a = start; a<=end; a++){
	  if (values[a] == NULL ){
	    struct vary_node * newNode = (struct vary_node *)malloc(sizeof(struct vary_node));
	    //printf("hi\n"); 
	    strcpy(newNode->name, op[i].op.vary.p->name); 
	    newNode->value = (double)(1/ (double) (end-start) )*(double)(a-start);
	    newNode->next = NULL;
	    //printf("a-start: %f \n",(double)(a-start));
	    //printf("end - start: %d \n", end-start);
	    //printf("Multiply: %f \n", 1/(end-start));
	    ///printf("%f\n", newNode->value);
	    //printf("a is %f, end is %f, start is %f end is %f, start is %f\n", a, endVal, startVal, start, end);

	    values[a] = newNode;
	  } else {
	    struct vary_node * oldNode = values[a];
	    
	    for (; oldNode->next!=NULL; oldNode=oldNode->next){}
	    
	    struct vary_node * newNode = (struct vary_node *)malloc(sizeof(struct vary_node));
	    strcpy(newNode->name, op[i].op.vary.p->name);
	    //newNode->value = (startVal-endVal)/(end-start)*(a-start);
	    newNode->value = (double)(1/ (double) (end-start) )*(double)(a-start);
	    newNode->next = NULL;
	    oldNode->next = newNode;
	  }
	}
	break;
      }
    }
    return values;
  }
  printf("Did not find any varies...\n");
  return NULL;
}


/*======== void print_knobs() ==========
Inputs:   
Returns: 

Goes through symtab and display all the knobs and their
currnt values

jdyrlandweaver
====================*/
void print_knobs() {
  
  int i;

  printf( "ID\tNAME\t\tTYPE\t\tVALUE\n" );
  for ( i=0; i < lastsym; i++ ) {

    if ( symtab[i].type == SYM_VALUE ) {
      printf( "%d\t%s\t\t", i, symtab[i].name );

      printf( "SYM_VALUE\t");
      printf( "%6.2f\n", symtab[i].s.value);
    }
  }
}


/*======== void my_main() ==========
  Inputs:
  Returns: 

  This is the main engine of the interpreter, it should
  handle most of the commadns in mdl.

  If frames is not present in the source (and therefore 
  num_frames is 1, then process_knobs should be called.

  If frames is present, the enitre op array must be
  applied frames time. At the end of each frame iteration
  save the current screen to a file named the
  provided basename plus a numeric string such that the
  files will be listed in order, then clear the screen and
  reset any other data structures that need it.

  Important note: you cannot just name your files in 
  regular sequence, like pic0, pic1, pic2, pic3... if that
  is done, then pic1, pic10, pic11... will come before pic2
  and so on. In order to keep things clear, add leading 0s
  to the numeric portion of the name. If you use sprintf, 
  you can use "%0xd" for this purpose. It will add at most
  x 0s in front of a number, if needed, so if used correctly,
  and x = 4, you would get numbers like 0001, 0002, 0011,
  0487

  05/17/12 09:41:35
  jdyrlandweaver
  ====================*/
void my_main( int polygons ) {

  int i, f, j, diffFrames;
  double step;
  double xval, yval, zval, knob_value;
  struct matrix *transform;
  struct matrix *tmp;
  struct stack *s;
  screen t;
  color g;

  SYMTAB * table;

  struct vary_node **knobs;
  struct vary_node *vn;
  char frame_name[128];
  char knobName[128];

  s = new_stack();
  tmp = new_matrix( 4, 1 );

  num_frames = 1;
  step = 5;
 
  g.red = 0;
  g.green = 250;
  g.blue = 250;
 

  first_pass();
  printf("Completed First Pass \n");
  printf("Num_Frames: %d\n", num_frames);
  knobs = second_pass();
  printf("Completed second pass\n\n");

  for (f = 0; f<num_frames; f++){

    clear_screen( t );
    //printf("Populating the table with values from the array\n");
    vn = knobs[f];
    for (vn = knobs[f]; vn!=NULL; vn=vn->next){
      //printf("hi\n");
      set_value( lookup_symbol( vn->name ), vn->value );
      //printf("hi\n");
    }

    print_knobs();
    s = new_stack();
    tmp = new_matrix( 4, 1 );
    
    for (i=0;i<lastop;i++) {
      switch (op[i].opcode) {
      case SPHERE:
	//printf("SPHERE \n");
	tmp = new_matrix( 4, 4 );
	ident(tmp);
	//printf("%f, %f, %f, %f", op[i].op.sphere.d[0], op[i].op.sphere.d[1], op[i].op.sphere.d[2], op[i].op.sphere.r);
	//exit(0);
	add_sphere( tmp, op[i].op.sphere.d[0], //cx
		    op[i].op.sphere.d[1],  //cy
		    op[i].op.sphere.d[2],  //cz
		    op[i].op.sphere.r,
		    step);
	//apply the current top origin
	matrix_mult( s->data[ s->top ], tmp );
	//print_matrix(tmp);
	draw_polygons( tmp, t, g );
	//display(t);
	tmp->lastcol = 0;
	break;

      case TORUS:
	//printf("TORUSED\n");
	add_torus( tmp, op[i].op.torus.d[0], //cx
		   op[i].op.torus.d[1],     //cy
		   op[i].op.torus.d[2],    //cz
		   op[i].op.torus.r0,
		   op[i].op.torus.r1,
		   step);
	matrix_mult( s->data[ s->top ], tmp );
	draw_polygons( tmp, t, g );
	tmp->lastcol = 0;
	break;

      case BOX:
	//printf("BOXED\n");

	add_box( tmp, op[i].op.box.d0[0],
		 op[i].op.box.d0[1],
		 op[i].op.box.d0[2],
		 op[i].op.box.d1[0],
		 op[i].op.box.d1[1],
		 op[i].op.box.d1[2]);
	matrix_mult( s->data[ s->top ], tmp );
	draw_polygons( tmp, t, g );
	tmp->lastcol = 0;
	//printf("BOXED-ed\n");
	break;

      case LINE:
	//printf("LINED\n");
	add_edge( tmp, op[i].op.line.p0[0],
		  op[i].op.line.p0[1],
		  op[i].op.line.p0[1],
		  op[i].op.line.p1[0],
		  op[i].op.line.p1[1],
		  op[i].op.line.p1[1]);
	draw_lines( tmp, t, g );
	tmp->lastcol = 0;
	break;

      case MOVE:
	//printf("MOVE\n");
	//get the factors
	if (op[i].op.move.p == NULL){
	  xval = op[i].op.move.d[0];
	  yval =  op[i].op.move.d[1];
	  zval = op[i].op.move.d[2];
	} else {
	  knob_value = lookup_symbol(op[i].op.move.p->name)->s.value;
	  xval = op[i].op.move.d[0] * knob_value;
	  yval = op[i].op.move.d[1] * knob_value;
	  zval = op[i].op.move.d[2] * knob_value;
	}
	  
	transform = make_translate( xval, yval, zval );
	//multiply by the existing origin
	matrix_mult( s->data[ s->top ], transform );
	//put the new matrix on the top
	copy_matrix( transform, s->data[ s->top ] );
	free_matrix( transform );
	break;

      case SCALE:
	//strncpy( knobName =

	if (op[i].op.scale.p == NULL){
	  xval = op[i].op.scale.d[0];
	  yval = op[i].op.scale.d[1];
	  zval = op[i].op.scale.d[2];
	} else {
	  knob_value = lookup_symbol(op[i].op.scale.p->name)->s.value;
	  xval = op[i].op.scale.d[0] * knob_value;
	  yval = op[i].op.scale.d[1] * knob_value;
	  zval = op[i].op.scale.d[2] * knob_value;
	}
	
	transform = make_scale( xval, yval, zval );
	matrix_mult( s->data[ s->top ], transform );
	//put the new matrix on the top
	copy_matrix( transform, s->data[ s->top ] );
	free_matrix( transform );
	break;

      case ROTATE:
	//printf("ROTATE\n");
	xval = op[i].op.rotate.degrees * ( M_PI / 180 );

	if (op[i].op.rotate.p == NULL){
	  knob_value = 1;
	} else {
	  knob_value = lookup_symbol(op[i].op.rotate.p->name)->s.value;
	}
	
	if ( op[i].op.rotate.axis == 0 ) 
	  transform = make_rotX( xval * knob_value);
	else if ( op[i].op.rotate.axis == 1 ) 
	  transform = make_rotY( xval * knob_value);
	else if ( op[i].op.rotate.axis == 2 ) 
	  transform = make_rotZ( xval * knob_value );

	matrix_mult( s->data[ s->top ], transform );
	//put the new matrix on the top
	copy_matrix( transform, s->data[ s->top ] );
	free_matrix( transform );
	break;

      case PUSH:
	//printf("PUSH\n");
	push( s );
	break;
      case POP:
	//printf("POP\n");
	pop( s );
	break;
      case SAVE:
	//printf("SAVE\n");
	save_extension( t, op[i].op.save.p->name );
	break;
      case DISPLAY:
	//printf("DISPLAY\n");
	display( t );
	break;
      }
      if (num_frames!=1){
	sprintf (frame_name, "./animate/%s%03d.png", name, f );
	save_extension( t, frame_name );
      }
    }
  
    free_stack( s );
    free_matrix( tmp );
    
    //free_matrix( transform );
    
  }
}

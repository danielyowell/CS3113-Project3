#include <stdio.h>
#include <stdlib.h>

int main () {
   FILE *fp;
   int c;
  
// fopen opens the file
// FILE objects have little bookmarks in them that automatically respond to our code
   fp = fopen("mytest.dat","r");
   while(1) {
// fgetc gets the next character
// the bookmark is moved up one character
      c = fgetc(fp);
// feof checks if the end of the file has been reached
// in other words, is the bookmark at the end?
      if( feof(fp) ) { 
         break ;
      }
// otherwise, print it
      printf("%c", c);
   }
   printf("\n");
// close the file
   fclose(fp);
   return(0);
}
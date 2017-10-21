/*******************************************************************************
 * test_motors.c
 *
 * James Strawson 2016
 * demonstrates use of H-bridges to drive motors. Instructions are printed
 * to the screen when called.
 *******************************************************************************/

#include "rc_usefulincludes.h"
#include "roboticscape.h"

// printed if some invalid argument was given
void print_usage(){
  printf("\n");
  printf("-d {duty}		define a duty cycle from -1.0 to 1.0\n");
  printf("-h			print this help message\n");
  printf("\n");
}


int main(int argc, char *argv[]){
  float duty = 0.5f;

  float motor_one_offset = 1.2;
  float motor_two_offset = 1.0;

  int time = 3;


  // sanity check cape library initialized
  if(rc_initialize()){
    printf("failed to initialize cape\n");
    return -1;
  }

  // bring H-bridges of of standby
  rc_enable_motors();
  rc_set_led(GREEN, ON);
  rc_set_led(RED, ON);

  printf("Starting motors\n\n");

  rc_set_motor(1, duty * motor_one_offset);
  rc_set_motor(2, duty * motor_two_offset);

  sleep(time);

  printf("Break\n\n");

  rc_set_motor_brake(1);
  rc_set_motor_brake(2);

  sleep(time);

  printf("Backwards\n\n");

  rc_set_motor(1, -duty * motor_one_offset);
  rc_set_motor(2, -duty * motor_two_offset);

  sleep(time);

  // User must have existed, put H-bridges into standby
  // not entirely necessary since cleanup_cape does this too
  rc_disable_motors();
  printf("All Motors Off\n\n");

  // final cleanup
  rc_cleanup();
  return 0;
}

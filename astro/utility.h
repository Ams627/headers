#pragma once
#define PI 3.14159265358979323846264338327950288419716939
void pdms(double x); 
void phms(double x); // Given hours as a decimal (0-24) - print hours minutes and seconds:

// from radians, print decimal degrees:
#define PDEG(x) std::cout << #x " is: " << (x) * 180.0 / PI << std::endl;
// from radians, print decimal seconds:
#define PSEC(x) PDEG(x * 3600);



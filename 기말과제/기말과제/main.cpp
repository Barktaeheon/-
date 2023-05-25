#include<iostream>
#include "Judgment.h"

#include "2018184040.h"
using namespace std;

int main(void)
{
	Judgment judgment;

	judgment.SetYourFunc(
		BlackAttack_2018184040/*선공*/,
		BlackDefence_2018184040,
		WhiteAttack_2018184040/*후공*/,
		WhiteDefence_2018184040
		);
	judgment.GamePlay();

}
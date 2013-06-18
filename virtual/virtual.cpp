// =====================================================================================
// 
//       Filename:  virtual.cpp
//
//    Description:  虚函数的非法访问
//
//        Version:  1.0
//        Created:  2013年05月10日 20时04分23秒
//       Revision:  none
//       Compiler:  g++
//
//         Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
//        Company:  Class 1107 of Computer Science and Technology
// 
// =====================================================================================

#include <iostream>

class PriVir
{
private:
	virtual void private_fun()
	{
		std::cout << "Private!!" << std::endl;
	}
};

int main(int argc, char *argv[])
{
	PriVir vir;

	typedef void (*pFun)(void);

	pFun pf = reinterpret_cast<pFun>(*reinterpret_cast<int *>(*reinterpret_cast<int *>(&vir)));

	pf();

	return 0;
}


//******************************************
// filename : linear fit
// author : lxw
// description : linear fit
// date : 20171221
// contact : QQ392134738
//******************************************

#ifndef _LINEAR_FIT_H_
#define _LINEAR_FIT_H_


#include "polynomialfit.h"
#include <iostream>
#include <math.h>

/*
 *��������GoodNess��������Ŷ�R
*/
class GoodNess
{
public:
    GoodNess():goodness(0.0){;}
    ~GoodNess(){;}

    void calcGoodNess(double *x,double *y,int count);
    double getGoodNess();

protected:
    virtual double func(double x) = 0;
    double goodness; // ����Ŷ�
};


/* Polynomial -- ����ʽ�����y=(Pn*x^n)+(Pn-1*x^n-1)+...+P1*x+P0
 * setAttribute: ���ú����Ľ��� , ���ߵĽؾ�
 * setSample: ������������������������ʹ��Ȩ�ء�ÿ��������Ȩ��
 * process: �������������ú����ϵ��������Ŷȣ����������úö���ʽ�Ľ�����������
*/
class Polynomial : public PolynomialFit , public GoodNess
{
public:
    Polynomial();
    ~Polynomial();

    bool setSample(double *x,
                  double *y,
                  int count ,
                  bool enableWeight ,
                  double *w ); // ��������
    bool process(); // �����������Ŷȵļ���

protected:
    double *osx , *osy ; // ԭʼ�������ݺ�����
    int ocount;
    void copy(double *x, double *y, int count);

    double func(double x); // ����
};


/* logarithm -- ������� y = P2*ln(x) + P1
 * setSample: ������������������������ʹ��Ȩ�ء�ÿ��������Ȩ��
 * process: �������������ú����ϵ��������Ŷȣ����������ú�������
*/
class Logarithm : public Polynomial
{
public:
    Logarithm();
    bool setSample(double *x,
                  double *y,
                  int count ,
                  bool enableWeight ,
                  double *w ); // ��������


    double func(double x);
};


/* Exponent -- ������� y=P2*exp(P1*x)
 * setSample: ������������������������ʹ��Ȩ�ء�ÿ��������Ȩ��
 * process: �������������ú����ϵ��������Ŷȣ����������ú�������
*/
class Exponent : public Polynomial
{
public:
    Exponent();
    bool setSample(double *x,
                  double *y,
                  int count ,
                  bool enableWeight ,
                  double *w ); // ��������

    double func(double x);
    float getResult(int y); // ϵ�������ҷֱ��Ӧ0��1��2...n
};

/* Power -- �ݺ������ y=P2*x^(P1)
 * setSample: ������������������������ʹ��Ȩ�ء�ÿ��������Ȩ��
 * process: �������������ú����ϵ��������Ŷȣ����������ú�������
*/
class Power : public Polynomial
{
public:
    Power();
    bool setSample(double *x,
                  double *y,
                  int count ,
                  bool enableWeight ,
                  double *w ); // ��������

    double func(double x);
    float getResult(int y); // ϵ�������ҷֱ��Ӧ0��1��2...n
};

#endif // _POLYNOMAIAL_FIT_H_

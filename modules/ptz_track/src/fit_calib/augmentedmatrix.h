//*******************************************************
// filename : augment matrix
// author : lxw
// description : Solving the first order equation group
// date : 20171221
// contact : QQ392134738
//*******************************************************

#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <math.h>
#include <memory.h>
#include <vector>
#include <algorithm>

// �����Ķ���--��߼���ľ���
/*
class Fraction
{
public:
    double numerator; // ����
    double denominator; // ��ĸ

    Fraction()
    {
        numerator = 1.0;
        denominator = 1.0;
    }

    Fraction(double numerator , double denominator)
    {
        this->numerator = numerator;
        this->denominator = denominator;
    }

    Fraction(double n)
    {
        numerator = n;
        denominator = 1.0;
    }

    // =
    inline void operator=(double numerator)
    {
        this->numerator = numerator;
        this->denominator = 1.0;
    }

    // =
    inline void operator=(Fraction &a)
    {
        this->numerator = a.numerator;
        this->denominator = a.denominator;
    }


    // -
    friend inline Fraction operator-(Fraction &a , Fraction &b)
    {
        double n = a.numerator*b.denominator - a.denominator*b.numerator;
        double d = a.denominator*b.denominator;
        return Fraction(n , d);
    }

    // +
    friend inline Fraction operator+(Fraction &a , Fraction &b)
    {
        Fraction ret ;
        ret.numerator = a.numerator*b.denominator + a.denominator*b.numerator;
        ret.denominator = a.denominator*b.denominator;
        return ret;
    }

    // -=
    inline void operator-=(Fraction &a)
    {
        numerator = a.denominator*numerator - a.numerator*denominator;
        denominator = a.denominator*denominator;
    }

    // *
    friend inline Fraction operator*(Fraction &a , Fraction &b)
    {
        double n = a.numerator*b.numerator;
        double d = a.denominator*b.denominator;
        return Fraction(n,d);
    }

    // x=
    inline void operator*=(Fraction &a)
    {
        numerator *= a.numerator;
        denominator *= a.denominator;
    }

    // /
    friend inline Fraction operator/(Fraction &a , Fraction &b)
    {
        Fraction ret ;
        ret.numerator = a.numerator*b.denominator;
        ret.denominator = a.denominator*b.numerator;
        return ret;
    }

    // ==
    inline bool operator==(Fraction &a)
    {
        return this->numerator*a.denominator==this->denominator*a.numerator;
    }

    // ==
    inline bool operator==(double a)
    {
        return this->numerator==this->denominator*a;
    }
};
*/

// ʹ��long double ��߼���ľ���
#if !defined (fraction)
typedef long double  fraction;
#endif


// class AugmentedMatrix �� ����������ø�˹��Ԫ�ķ������
// һ�η����飬����������ϵĽ��������Ķ���-�����������ͼ��ʾ
//-|------------->x
// |
// |
// |
// |
// \/
// y
// �������ݺ������У�������ʾ
// |--------matrix-------|--augment--|----result----|
// |--2a + 3b + 11c + 5d =  2 -------|--1 0 0 0|-2--|
// |--1a +  b +  5c + 2d =  1 -------|--0 1 0 0| 0--|
// |--2a +  b +  3c + 2d = -3 -------|--0 0 1 0| 1--|
// |--1a +  b +  3c + 3d = -3 -------|--0 0 0 1|-1--|
class AugmentedMatrix
{
public:
    AugmentedMatrix(); //���캯��1
    ~AugmentedMatrix(); //��������

    bool setAugmentData(fraction *values, int height); //����������()
    bool setMatrixData(fraction *valves, int width, int height); // ��������������()

    inline fraction getResult(int y){return augment[y];}
    bool process(); // ��������
    void print(); // ��ӡ���

private:
    fraction *matrix[32]; // ��������--����ɷ�����ʽ��߾���(���32��)
    fraction augment[32]; // ���ӵ�һ��
    int width , height; // ����Ŀ��

    void releaseData(); // �ͷ��ڴ�
    bool augmentedMatrixTrans(); // ��˹��Ԫ��������Ԫ�������ֵ
    bool sortLines(int p ); // ����
    void elimination( int p ); // ��Ԫ
    void swap(fraction *a , fraction *b); // ��������
    void elimination_inv( int x ); // ��Ԫ
    void identityMatrix(); //��Ϊ��λ����
};

#endif // _MATRIX_H_

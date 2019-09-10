//******************************************
// filename : polynomial fit
// author : lxw
// description : polynomial fit
// date : 20171221
// contact : QQ392134738
//******************************************


#ifndef _POLYNOMAIAL_FIT_H_
#define _POLYNOMAIAL_FIT_H_

#include "augmentedmatrix.h"

class PolynomialFit
{
public:
    PolynomialFit();
    ~PolynomialFit();

    //���ö���ʽ�����ԣ��纯���Ľ��� , ���ߵĽؾ��
    void setAttribute(int degree ,
                      bool intercedeEnable = false,
                      double intercede = 0.0 );

    //�������ߵ�������Ȩ�ص�
    virtual bool setSample(double *x ,
                           double *y ,
                           int count ,
                           bool enableWeight  ,
                           double *w );

    virtual float getResult(int y); // ��ȡ����Ľ��
    virtual bool process();
    void print(); // ��ӡ


protected:
    double intercede; // �ؾ�
    bool isIntercedeEnable; // �ؾ�ʹ��
    int degree; // ����
    int multiterm; // ����ʽ������
    fraction *sx , *sy , *weight; // ����-Ȩ��
    int count; // ��������

private:
    fraction *matrix , *rvector , *results; // ϵ������ͽ������

    bool leastQuare(); // ������С���˷���������ת�������ķ�����
    fraction calcMatrixUnit(int x , int y); // �������ĳ����ۼ�ֵ
    void fillVectorUnit(int y); // ��������ĳ����ۼ�ֵ

    void init();
    void releaseSamples(); // �ͷ��ڴ�
    void releaseMatrix();

};

#endif // _POLYNOMAIAL_FIT_H_

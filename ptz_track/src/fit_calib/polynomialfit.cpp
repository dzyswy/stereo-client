#include "polynomialfit.h"
#include <math.h>
#include <memory.h>
#include <iostream>

//���캯��1
PolynomialFit::PolynomialFit()
{
    init();
}

// ��������
PolynomialFit::~PolynomialFit()
{
    releaseSamples();
    releaseMatrix();
}

/**
 * @brief PolynomialFit::setAttribute ���ö���ʽ������
 * @param degree ���ö���ʽ�Ľ���(degree�벻Ҫ����32)
 * @param intercedeEnable �����Ƿ�ʹ�ܽؾ�
 * @param intercede ����ʽ���ߵĽؾ�
 */
void PolynomialFit::setAttribute(int degree , bool intercedeEnable, double intercede )
{
    this->isIntercedeEnable = intercedeEnable;

    if(this->isIntercedeEnable){
        this->multiterm = degree;
        this->intercede = intercede;
    }else{
        this->multiterm = degree+1;
        this->intercede = 0.0;
    }
    this->degree = degree;

}

//��ʼ��
void PolynomialFit::init()
{
    multiterm = count = degree =0;
    sx =  sy = weight = NULL;

    intercede = 0.0 ;
    isIntercedeEnable = false;

    matrix  = rvector = results = NULL; // ϵ������ͽ������
}

/**
 * @brief PolynomialFit::setSample ������Ҫ������ߵ�������Ȩ�ص�
 * @param x ����x��
 * @param y ����y��
 * @param count ��������
 * @param enableWeight �Ƿ�ʹ��Ȩ��
 * @param w ������Ȩ��
 * @return �����Ƿ�������
 */
bool PolynomialFit::setSample(double *x,
                              double *y,
                              int count ,
                              bool enableWeight ,
                              double *w )
{
    releaseSamples();
    this->count = count;
    this->sx = new fraction[count] , this->sy = new fraction[count] ,\
            this->weight = new fraction[count];

    for(int i(0) ; i < count ; i++)
    {
        this->sx[i] = x[i] ;
        this->sy[i] = y[i] ;
        if(enableWeight)
            this->weight[i] = w[i];
        else
            this->weight[i] = 1.0;
    }

    return true;
}

// ��ȡ����Ľ��
float PolynomialFit::getResult(int y)
{
    return (float)results[y];
}

// ���ݴ���
bool PolynomialFit::process()
{
    // ������С���˷���һ�η�����
    if(!leastQuare())
        return false;

    // ͨ����˹���ⷽ����
    AugmentedMatrix am;
    am.setMatrixData( matrix , multiterm , multiterm ) ;
    am.setAugmentData( rvector , multiterm);
    if( !am.process() )
        return false;

    // ����Ľ�����Ƕ���ʽ��ϵ��
    for(int i=0;i<multiterm;i++){
        results[i] = am.getResult(i);
    }

    return true;
}

// ��ӡ���
void PolynomialFit::print()
{
	int y = 0;
    std::cout << "\nPolynomial Fit :" << std::endl;
    for( y = 0 ; y<multiterm ; y++ )
    {
        for( int x = 0 ; x<multiterm ; x++ )
        {
            std::cout << (float)matrix[y*multiterm + x] << "\t";
        }
        std::cout << (float)rvector[y] << std::endl;
    }

    std::cout << "\nresults:" << std::endl;
    for( y = 0 ; y<multiterm ; y++ )
    {
        std::cout << (float)getResult(y) << "\t";
    }
    std::cout << std::endl;
}

// �ͷ��ڴ�
void PolynomialFit::releaseSamples()
{
    if(sx){
        delete [] sx;
        sx = NULL ;
    }
    if(sy){
        delete [] sy;
        sy = NULL ;
    }
    if(weight){
        delete weight;
        weight = NULL;
    }
    count  = 0 ; // ��������

}

// �ͷž����ڴ�
void PolynomialFit::releaseMatrix()
{
    if(matrix ){
        delete [] matrix ;
        matrix = NULL;
    }

    if(rvector){
        delete [] rvector;
        rvector = NULL;
    }

    if(results){
        delete [] results;
        results = NULL;
    }
}

// ������С���˷���������ת�������ķ�����
bool PolynomialFit::leastQuare()
{
    int x = 0 , y = 0;
    if(multiterm > count || count ==0 || multiterm == 0)
        return false;

    releaseMatrix();
    matrix = new fraction[multiterm*multiterm];
    rvector = new fraction[multiterm];
    results = new fraction[multiterm];

    //�ԽǾ��󣬼���������е�ֵ
    for(int i=0 ; i < 2*multiterm-1 ; i++)
    {
        if( i < multiterm){
            x = i;
            y = 0;
        }else{
            x = multiterm - 1;
            y = i - x;
        }

        fraction r = calcMatrixUnit(x, y);
        for(int yi=0;yi<=i;yi++)
        {
            int xi = i-yi;
            if(yi < multiterm && xi < multiterm)
                matrix[yi*multiterm + xi] = r;
        }
    }

    // ���������е�ֵ
    for(y=0;y<multiterm;y++)
        fillVectorUnit(y);
    return true;
}

// �������ĳ����ۼ�ֵ
fraction PolynomialFit::calcMatrixUnit(int x, int y)
{
    fraction sum = 0.0;
    double n = 2*degree - x - y  ;
    for(int i=0;i<count;i++)
    {
        sum += pow( (double)sx[i] , n ) * weight[i];
    }

    return sum;
}

// ��������ĳ����ۼ�ֵ
void PolynomialFit::fillVectorUnit(int y)
{
    rvector[y] = 0.0;
    double n = degree - y;
    for(int i=0;i<count;i++)
    {
        rvector[y] += (sy[i]-intercede) * pow( (double)sx[i] , n ) * weight[i];
    }
}


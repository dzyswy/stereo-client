
#include "linearfit.h"


/////////////////////////////////////////////////////////////////////////
/**
 * @brief GoodNess::calcGoodNess ��������Ŷ�(��֪��Ϊʲôֻ�ж���ʽ���ŶȺ�Excel�ļ�����һ��)
 * @param x ����x
 * @param y ����y
 * @param count ��������
 */
void GoodNess::calcGoodNess(double *x, double *y, int count)
{
    double sum0 = 0.0,dy=0.0;
    double ESS = 0.0; // �ع�ƽ����
    double RSS = 0.0; // �в�ƽ����
    double TSS = 0.0; // ����ƽ����
	int i = 0;

    for(i=0;i<count;i++)
        sum0 += y[i];
    dy = sum0/count;

    for(i=0;i<count;i++)
    {
        ESS += pow(func(x[i]) - dy, 2);
        RSS += pow(y[i] - func(x[i]) ,2);
    }
    TSS = ESS + RSS;
    if(TSS == 0.0)
        goodness = 0.0;
    else
        goodness = ESS / TSS ;
}

double GoodNess::getGoodNess()
{
    return goodness;
}


/////////////////////////////////////////////////////////////////////////
/**
 * ����ʽ  y=(Pn*x^n)+(Pn-1*x^n-1)+...+P1*x+P0
 */
// ���캯��
Polynomial::Polynomial() :
    PolynomialFit() ,
    GoodNess() ,
    osx(NULL) ,
    osy(NULL) ,
    ocount(0)
{
}

// ��������
Polynomial::~Polynomial()
{
    if(osx != NULL)
        delete [] osx;

    if(osy != NULL)
        delete [] osy;
}

//��������--x:x�����ݣ�y:y�����ݣ�w:wȨ��ϵ����count��������
bool Polynomial::setSample(double *x,
                              double *y,
                              int count ,
                              bool enableWeight ,
                              double *w )
{
    copy(x, y, count);
    return PolynomialFit::setSample(x,y,count,enableWeight ,w);
}

// ���ߵ�ԭ��--y=(Pn*x^n)+(Pn-1*x^n-1)+...+P1*x+P0
double Polynomial::func(double x)
{
    double result = 0;
    if(!isIntercedeEnable){
        for(int i(0) ; i<=degree ; i++)
        {
            result += getResult(i)*pow(x , degree-i);
        }
    }else{
        for(int i(0) ; i<degree ; i++)
        {
            result += getResult(i)*pow(x , degree-i);
        }
        result += intercede;
    }

    return result;
}

// �����������Ŷȵļ���
bool Polynomial::process()
{
    bool ret = PolynomialFit::process();
    if(ret == false)
        return false;

    calcGoodNess(osx,osy,ocount);
    return true;
}

//��ȡһ������
void Polynomial::copy(double *x, double *y, int count)
{
    ocount = count;
    osx = new double[count] , osy = new double[count];
    for(int i(0) ; i < count ; i++)
    {
        osx[i] = x[i] ;
        osy[i] = y[i] ;
    }
}

//////////////////////////////////////////////////////////////////////////////
/**
 * ����  y = P2*ln(x) + P1
 */
Logarithm::Logarithm()
    : Polynomial()
{
    setAttribute(1,false);
}


bool Logarithm::setSample(double *x, double *y, int count, bool enableWeight, double *w)
{
    //��ȡһ������
    copy(x,y,count);

    //����
    double *px = new double[count];
    for(int i(0) ; i<count ; i++){
        if(x[i] <= 0.0){
            delete [] px;
            return false;
        }

        px[i] = log(x[i]);
    }

    bool ret = PolynomialFit::setSample(px,y,count,enableWeight,w);
    delete [] px;
    return ret;
}

double Logarithm::func(double x)
{
    return getResult(0) * log(x) + getResult(1);
}


//////////////////////////////////////////////////////////////////////////////
/**
 * ָ�� y = P2*exp(P1*x)
 */
Exponent::Exponent() :
    Polynomial()
{
    setAttribute(1,false);
}

//��������
bool Exponent::setSample(double *x, double *y, int count, bool enableWeight, double *w)
{
    //��ȡһ������
    copy(x,y,count);

    //����
    double *py = new double[count];
    for(int i(0) ; i<count ; i++){
        if(y[i] <= 0.0){
            delete [] py;
            return false;
        }

        py[i] = log(y[i]);
    }

    bool ret = PolynomialFit::setSample(x,py,count,enableWeight,w);
    delete [] py;
    return ret;
}

//
double Exponent::func(double x)
{
    return getResult(0) * exp(x*getResult(1));
}

// ϵ�������ҷֱ��Ӧ0��1��2...y
float Exponent::getResult(int y)
{
    if(y==0)
        return exp(PolynomialFit::getResult(1)) ;
    else
        return PolynomialFit::getResult(0);
}


//////////////////////////////////////////////////////////////////////////////
/**
 * �ݺ��� y = P2*x^P1
 */
Power::Power() : Polynomial()
{
    setAttribute(1,false);
}

//��������
bool Power::setSample(double *x, double *y, int count, bool enableWeight, double *w)
{
    //��ȡһ������
    copy(x,y,count);

    //����
    double *px = new double[count];
    double *py = new double[count];
    for(int i(0) ; i<count ; i++){
        if(y[i] <= 0.0 || x[i] <= 0.0){
            delete [] px;
            delete [] py;
            return false;
        }

        px[i] = log(x[i]);
        py[i] = log(y[i]);
    }

    bool ret = PolynomialFit::setSample(px,py,count,enableWeight,w);
    delete [] px;
    delete [] py;
    return ret;
}

//
double Power::func(double x)
{
    return getResult(0) * pow(x ,(double)getResult(1));
}

// ϵ�������ҷֱ��Ӧ0��1��2...y
float Power::getResult(int y)
{
    if(y==0)
        return exp(PolynomialFit::getResult(1)) ;
    else
        return PolynomialFit::getResult(0);
}


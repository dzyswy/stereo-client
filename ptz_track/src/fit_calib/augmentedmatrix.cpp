#include "augmentedmatrix.h"
#include <iostream>

//���캯��1
AugmentedMatrix::AugmentedMatrix()
{
    width = height = 0;
}

//��������
AugmentedMatrix::~AugmentedMatrix()
{
    releaseData();
}

//����������
bool AugmentedMatrix::setMatrixData(fraction *valves , int width, int height)
{
    this->width = width;
    this->height = height;

    for(int i=0 ; i<this->height ; i++)
    {
        fraction *x = new fraction[this->width];
        for(int j=0 ; j<this->width ; j++)
            x[j] = valves[i*this->width + j];
        matrix[i] = x;
    }

    return true;
}

// ��������
bool AugmentedMatrix::process()
{
    return augmentedMatrixTrans();
}

 // ��ӡ���
void AugmentedMatrix::print()
{
    std::cout << "\nAugmented Matrix :" << std::endl;
    for( int y = 0 ; y<height ; y++ )
    {
        for( int x = 0 ; x<width ; x++ )
        {
            std::cout << (float)matrix[y][x] << "\t";
        }
        std::cout << (float)augment[y] << std::endl;
    }
}

// ��������������
bool AugmentedMatrix::setAugmentData( fraction *values , int height)
{
    this->height = height;
    for( int i(0) ; i < this->height ; i++)
        augment[i]  = values[i];
    return true;
}

//���������Ԫ�任
bool AugmentedMatrix::augmentedMatrixTrans()
{
    if(width != height)
        return false;

    for(int p=0 ; p<width ; p++)
    {
        if(!sortLines(p))// ����
            return false;
        elimination(p);// ��Ԫ
    }

    // ���Ͻǵ���Ԫ�����
    for(int x=width-1 ; x>=0 ; x--)
    {
        if(matrix[x][x] == 0.0)
            return false;
        elimination_inv(x);
    }
    identityMatrix();

    return true;
}

// ���򣬷�ֹ(x,y)���괦��ֵΪ0��pΪ������к��У���ʱ������ͬ��
// ����false�����������޽�
bool AugmentedMatrix::sortLines(int p)
{
    for( int y = p ; y<height ; y++ )
    {
        if( matrix[y][p] == 0.0 )
        {
            if( y == height-1 ) //ȫ��Ϊ0
                return false;
        }else{
            if( y != p )
                swap(matrix[y] , matrix[p]);
            break;
        }
    }

    return true;
}

// ��˹��Ԫ��pΪ������к��У���ʱ������ͬ��
void AugmentedMatrix::elimination(int p)
{
    for( int y = p+1 ; y<height ; y++ )
    {
        fraction t = matrix[p][p] / matrix[y][p];
        //matrix[y][p] = 0.0;
        for( int x = p ; x<width ; x++ )
        {
            matrix[y][x] = matrix[y][x]*t - matrix[p][x];
        }
        augment[y] = augment[y]*t - augment[p];
    }
}

// ��������
void AugmentedMatrix::swap(fraction *a, fraction *b)
{
    fraction *pF = a;
    a = b;
    a = pF;
}

// ���Ͻǵ���Ԫ�����
void AugmentedMatrix::elimination_inv(int x)
{
    for( int y = 0 ; y<x ; y++ )
    {
        if(matrix[y][x] == 0.0)
            continue;

        fraction t = matrix[y][x] / matrix[x][x];
        matrix[y][x] = 0.0;
        augment[y] = augment[y] - augment[x]*t;
    }
}

//��Ϊ��λ����
void AugmentedMatrix::identityMatrix()
{
    for( int p = 0 ; p<width ; p++ )
    {
        if(matrix[p][p] == 0.0)
            continue;

        augment[p] /= matrix[p][p];
        matrix[p][p] = 1.0;
    }
}

// �ͷ��ڴ�
void AugmentedMatrix::releaseData()
{
     for(int i(0) ; i < height ; i++)
     {
         if(matrix[i] != NULL )
             delete [] matrix[i];
     }

     width = height = 0;
}

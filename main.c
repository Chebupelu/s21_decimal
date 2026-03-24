#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>

typedef struct 
{
    int bits[4];
} s21_decimal ;


int get_bit(s21_decimal value, int index){
    return (value.bits[index / 32] >> (index %32)) & 1;
}

void set_bit(s21_decimal *value, int index, int bit){
    if(bit){
        value -> bits[index / 32] |= (1U << (index % 32));
    }
    else{
        value -> bits[index / 32] &= ~(1U << (index % 32));
    }
}

int get_sign(s21_decimal value){
    return (value.bits[3] >> 31) &1;
}

void set_sign(s21_decimal *value, int sign){
    if(sign){
        value -> bits[3] |= (1u << 31);
    }
    else{
        value -> bits[3] &= ~(1u << 31);
    }
}

int get_scale(s21_decimal value){
    return (value.bits[3] >> 16) & 0xFF;
}

void set_scale(s21_decimal *value, int scale){
    value -> bits[3] &= ~(0xFF << 16);

    value -> bits[3] |= (scale & 0xFF) << 16;
}

int is_zero(s21_decimal value){
    return( 
            value.bits[0] == 0&&
            value.bits[1] == 0 &&
            value.bits[2] == 0 &&
            value.bits[3] == 0);
}

int compare_mantissa(s21_decimal value1, s21_decimal value2){
    int result = 0;

    for (int i = 2; i >= 0; i--)
    {
        if(value1.bits[i] > value2.bits[i]){
            return 1;
            break;
        }
        else if (value1.bits[i] < value2.bits[i]){
            return -1;
            break;
        }
    }
    return result;
}

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result){
    int carry = 0;

    memset(result, 0, sizeof(s21_decimal));

    for (int i = 0; i < 96; i++)
    {
        int bit1 = get_bit(value_1, i);
        int bit2 = get_bit(value_2, i);

        int sum = bit1 + bit2 + carry;

        set_bit(result, i, sum % 2);
        carry = sum / 2;
    }

    if(carry) return 1;

    return 0;
    
}

void print_decimal_bits(s21_decimal value){
    for (int i = 127; i >= 0; i--)
    {
        printf("%d", get_bit(value, i));
        if(i % 32 == 0) printf(" ");
    }
    printf("\n");
    
}

int main(){
    
    s21_decimal val1 = {{0, 0, 0, 0}};
    s21_decimal val2 = {{0, 0, 0, 0}};
    s21_decimal res = {{0, 0, 0, 0}};

    set_bit(&val1, 0, 1);
    set_bit(&val2, 2, 1);

    printf("Число 1:" ); print_decimal_bits(val1);
    printf("Число 2:" ); print_decimal_bits(val2);

    int status = s21_add(val1, val2, &res);

    if (status == 0)
    {
        printf("Результат:" );
        print_decimal_bits(res);
    }
    else{
        printf("Ошибка переполнения!\n");
    }
    

    return 0;
    
}
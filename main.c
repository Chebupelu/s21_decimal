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

int shift_left(s21_decimal *value){
    int last_bit_0 = get_bit(*value, 31);
    int last_bit_1 = get_bit(*value, 63);
    int overflow = get_bit(*value, 95);

    for (int i = 0; i < 3; i++)
    {
        value -> bits[i] = (unsigned int)value->bits[i] << 1;
    }

    if(last_bit_0) set_bit(value, 32, 1);
    if(last_bit_0) set_bit(value, 64, 1);

    return overflow;
    
}

int add_mantissa(s21_decimal val1, s21_decimal val2, s21_decimal *result) {
    int carry = 0;
    // Обнуляем только первые 3 инта мантиссы
    for(int i = 0; i < 3; i++) result->bits[i] = 0;

    for (int i = 0; i < 96; i++) {
        int b1 = get_bit(val1, i);
        int b2 = get_bit(val2, i);
        int sum = b1 + b2 + carry;
        
        set_bit(result, i, sum % 2);
        carry = sum / 2;
    }
    return carry; // Возвращаем 1, если произошло переполнение мантиссы
}

int multiply_by_10(s21_decimal *value){
    s21_decimal v_x2 = *value;
    s21_decimal v_x8 = *value;

    shift_left(&v_x2); // x2

    shift_left(&v_x8); // x2
    shift_left(&v_x8); // x4
    shift_left(&v_x8); // x8

    return add_mantissa(v_x2, v_x8, value);
}

void leveling(s21_decimal *val1, s21_decimal *val2){
    int sc1 = get_scale(*val1);
    int sc2 = get_scale(*val2);

    if (sc1 < sc2) {
        for (int i = 0; i < (sc2 - sc1); i++) multiply_by_10(val1);
        set_scale(val1, sc2);
    } else if (sc2 < sc1) {
        for (int i = 0; i < (sc1 - sc2); i++) multiply_by_10(val2);
        set_scale(val2, sc1);
    }
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
    // Обнуляем результат
    memset(result, 0, sizeof(s21_decimal));

    // А) Выравниваем степени (чтобы складывать 0.10 и 0.01 как 10 + 1)
    leveling(&value_1, &value_2);

    int sign1 = get_sign(value_1);
    int sign2 = get_sign(value_2);
    int carry = 0;

    if (sign1 == sign2) {
        // Б) Если знаки одинаковые — просто складываем мантиссы
        carry = add_mantissa(value_1, value_2, result);
        set_sign(result, sign1);
        set_scale(result, get_scale(value_1));
    } else {
        // В) Если знаки разные — нужно вызывать вычитание (sub)
        // Но для начала можно реализовать только сложение положительных
    }

    // Если был carry в 96-м бите — это переполнение (для decimal это ошибка)
    return carry ? (sign1 == 0 ? 1 : 2) : 0; 
    
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
    set_scale(&val2, 2);

    printf("Число 1 (0.1): "); print_decimal_bits(val1);
    printf("Число 2 (0.05):"); print_decimal_bits(val2);

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
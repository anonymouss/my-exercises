#!/usr/bin/env python3

"""
    个税计算器（上海） - 2019

    `年度应纳税所得 = 年度综合所得 – 基本减除费用 – 专项扣除 – 专项附加扣除 – 其他扣除`

    2021年之前为过渡期，过渡期内一次性奖金收入可以计入年度综合所得计税也可以单独计算（和以前一样）

    年度综合所得 = 工资 + 其他
    一次性奖金 = 奖金 + 其他
"""


import os, sys
from decimal import Decimal


"""
    税率计算
    Args:
        taxPayable: 应纳税额
        annual: 是否安年度计算
"""
def taxCalculate(taxPayable, annual=False):
    base = 1
    if annual:
        base = 12

    if Decimal(taxPayable) <= Decimal(36000):
        return Decimal(taxPayable) * Decimal(0.03)
    elif Decimal(taxPayable) <= Decimal(144000):
        return Decimal(taxPayable) * Decimal(0.1) - Decimal(210 * base)
    elif Decimal(taxPayable) <= Decimal(300000):
        return Decimal(taxPayable) * Decimal(0.2) - Decimal(1410 * base)
    elif Decimal(taxPayable) <= Decimal(420000):
        return Decimal(taxPayable) * Decimal(0.25) - Decimal(2660 * base)
    elif Decimal(taxPayable) <= Decimal(660000):
        return Decimal(taxPayable) * Decimal(0.3) - Decimal(4410 * base)
    elif Decimal(taxPayable) <= Decimal(960000):
        return Decimal(taxPayable) * Decimal(0.35) - Decimal(7160 * base)
    else:
	    return Decimal(taxPayable) * Decimal(0.45) - Decimal(15160 * base)


if __name__ == '__main__':
    print('**** 收入部分 ****')
    salary = Decimal(input('税前月薪：'))
    bonus = Decimal(input('年终奖：'))
    other_salary = Decimal(input('其他希望计入年度综合所得收入（无输入0）：'))
    other_bonus= Decimal(input('其他希望计入一次性奖金收入（无输入0）：'))

    print('**** 五险一金 ****')
    last_base = Decimal(input('当前五险一金基准（不知输0）：'))
    new_base = Decimal(input('上年度平均月收入（与新五险一金基准相关，不知输0）：'))
    funds_rate = Decimal(input('自缴公积金比例（含补充，如0.12，即7% + 5%补充：）'))

    print('**** 专项扣除 ****')
    children = Decimal(input('子女教育月扣除（无输入0）：'))
    education = Decimal(input('继续教育月扣除（无输入0）：'))
    disease = Decimal(input('大病医疗月扣除（无输入0）：'))
    loan = Decimal(input('房贷专项月扣除（无输入0）：'))
    rent = Decimal(input('房租专项月扣除（无输入0）：'))
    elder = Decimal(input('赡养老人月扣除（无输入0）'))

    base_deduction = 60000

    pension_rate = Decimal(0.08) # 养老金比例 - 个人
    unemployment_rate = Decimal(0.005) # 失业保险 - 个人
    medicare_rate = Decimal(0.02) # 医疗保险 - 个人
    # 工伤与生育保险无个人部分

    total_salary = Decimal((salary * 12) + other_salary)
    total_bouns = Decimal(bonus + other_bonus)
    total_income = Decimal(total_salary + total_bouns)

    shanghai_base = 21396

    average_income = total_income / 12
    if average_income > shanghai_base:
        average_income = shanghai_base

    if last_base == 0 and new_base == 0:
        # 都不知道？只好取今年的了，不准确
        last_base = average_income
        new_base = average_income
    elif last_base == 0:
        # 当前的不知道？只好都取上年平均工资了，不准确
        if new_base > shanghai_base:
            new_base = shanghai_base
        last_base = new_base
    elif new_base == 0:
        # 上年不知道？只好新的取今年的了
        new_base = average_income
    # else:
    #   最准确，但由于今年社保基数未公布，任有一点偏差

    deduction_rate = funds_rate + pension_rate + unemployment_rate + medicare_rate
    special_deduction = (children + education + disease + loan + rent + elder) * 12
    social_deduction = (last_base + new_base) * deduction_rate * 6
    total_deduction = base_deduction + special_deduction + social_deduction

    print('总税前收入： %d' % total_income)

    print('------------ 方案 1 ：奖金收入并入年度综合所得计算 ------------')
    totalTax1 = taxCalculate(total_income - total_deduction, True)
    print(' - 总税额： %.2f' % totalTax1)
    print(' - 到手： %.2f' % Decimal(total_income - social_deduction - totalTax1))

    print('------------ 方案 2 ：奖金收入单独计算 -----------------------')
    salaryTax = taxCalculate(total_salary - total_deduction, True)
    bonusTax = taxCalculate(total_bouns)
    totalTax2 = Decimal(salaryTax + bonusTax)
    print(' - 总税额： %.2f' % totalTax2)
    print(' - 到手： %.2f' % Decimal(total_income - social_deduction - totalTax2))
    print('-----------------------------------------------------------')
    if totalTax1 < totalTax2:
        print('方案 1 靠谱')
    elif totalTax1 > totalTax2:
        print('方案 2 靠谱')
    else:
        print('都一样')
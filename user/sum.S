.data

msg_r_err:     .asciz "Ошибка чтения ввода.\n"
msg_f_err:   .asciz "Некорректный формат данных.\n"
output:            .asciz "%d\n"

input: .byte 100

.text
.global main

main:
    la a0, input
    li a1, 100
    call gets

    beqz a0, r_err

    la a0, input
    li a1, 0x20            # '\n'
    call strchr

    beqz a0, f_err

    sb zero, 0(a0)
    addi t0, a0, 1

    la a0, input
    call atoi
    mv t1, a0

    mv a0, t0
    call atoi
    mv t2, a0


    add t3, t1, t2
    la a0, output
    mv a1, t3
    call printf

    li a7, 2
    li a0, 0
    ecall

r_err:
    li a7, 16
    li a0, 2
    la a1, msg_r_err
    li a2, 50
    ecall

    li a7, 2
    li a0, 1
    ecall

f_err:
    li a7, 16
    li a0, 2
    la a1, msg_f_err
    li a2, 50
    ecall

    li a7, 2
    li a0, 1
    ecall
# finds factorial of a number using recursion

main:
addi s1, zero, 3    # s1 <- n
addi s1, s1, 1
addi t0, zero, 1
addi s2, zero, 1    # s2 <- ans
for1:
bge t0, s1, for1_end
add a0, zero, s2
add a1, zero, t0
jal ra, multiply
add s2, zero, a0
addi t0, t0, 1
jal t4, for1
for1_end:
sw s2, 0(zero)
jal ra, main_end

multiply:
# intialise
addi t3, zero, 0
add a2, zero, a0
addi a0, zero, 0
for2:
bge t3, a1, for2_end
add a0, a0, a2
addi t3, t3, 1
jal t4, for2
for2_end:
jalr ra, 0(ra)

main_end:
# end of execution
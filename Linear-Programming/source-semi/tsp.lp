\ Model TSP
\ LP format - for model browsing. Use MPS format to capture full model detail.
Minimize
  obj_and_sum
Subject To
 R0: - C0 - C1 + obj_cost[0] = 0
 R1: - C2 - C3 + obj_cost[1] = 0
 R2: - C4 - C5 + obj_cost[2] = 0
 R3: - C6 - C7 + obj_cost[3] = 0
 R4: - C8 - C9 + obj_cost[4] = 0
 R5: - C10 - C11 + obj_cost[5] = 0
 R6: - C12 - C13 + obj_cost[6] = 0
 obj_and_sum: - obj_and[0] - obj_and[1] - obj_and[2] - obj_and[3]
   - obj_and[4] - obj_and[5] - obj_and[6] + obj_and_sum = 0
 obj_cost_sum: - obj_cost[0] - obj_cost[1] - obj_cost[2] - obj_cost[3]
   - obj_cost[4] - obj_cost[5] - obj_cost[6] + obj_cost_sum = 0
 R9: C0 + C2 = 1
 R10: C8 + C10 = 1
 R11: C1 + C3 = 1
 R12: C9 + C11 = 1
 R13: - C0 + C4 + C6 = 0
 R14: C4 + C6 = 1
 R15: - C1 + C5 + C7 = 0
 R16: C5 + C7 <= 1
 R17: - C4 + C8 - C12 = 0
 R18: C8 <= 1
 R19: - C5 + C9 - C13 = 0
 R20: C9 = 1
 R21: - C6 + C10 = 0
 R22: C10 <= 1
 R23: - C7 + C11 = 0
 R24: C11 <= 1
 R25: - C2 + C12 = 0
 R26: C12 <= 1
 R27: - C3 + C13 = 0
 R28: C13 <= 1
 R29: C30 = 1
 R30: C36 = 6
 R31: C31 = 1
 R32: C37 = 6
 R33: 6 C0 + C30 - C32 <= 5
 R34: 6 C1 + C31 - C33 <= 5
 R35: 6 C2 + C30 - C40 <= 5
 R36: 6 C3 + C31 - C41 <= 5
 R37: 6 C4 + C32 - C34 <= 5
 R38: 6 C5 + C33 - C35 <= 5
 R39: 6 C6 + C32 - C38 <= 5
 R40: 6 C7 + C33 - C39 <= 5
 R41: 6 C8 + C34 - C36 <= 5
 R42: 6 C9 + C35 - C37 <= 5
 R43: 6 C10 - C36 + C38 <= 5
 R44: 6 C11 - C37 + C39 <= 5
 R45: 6 C12 - C34 + C40 <= 5
 R46: 6 C13 - C35 + C41 <= 5
Bounds
 1 <= C30 <= 6
 1 <= C31 <= 6
 1 <= C32 <= 6
 1 <= C33 <= 6
 1 <= C34 <= 6
 1 <= C35 <= 6
 1 <= C36 <= 6
 1 <= C37 <= 6
 1 <= C38 <= 6
 1 <= C39 <= 6
 1 <= C40 <= 6
 1 <= C41 <= 6
Binaries
 C0 C1 C2 C3 C4 C5 C6 C7 C8 C9 C10 C11 C12 C13 obj_and[0] obj_and[1]
 obj_and[2] obj_and[3] obj_and[4] obj_and[5] obj_and[6]
Generals
 obj_cost[0] obj_cost[1] obj_cost[2] obj_cost[3] obj_cost[4] obj_cost[5]
 obj_cost[6] obj_and_sum obj_cost_sum C30 C31 C32 C33 C34 C35 C36 C37 C38
 C39 C40 C41
General Constraints
 GC0: obj_and[0] = AND ( C0 , C1 )
 GC1: obj_and[1] = AND ( C2 , C3 )
 GC2: obj_and[2] = AND ( C4 , C5 )
 GC3: obj_and[3] = AND ( C6 , C7 )
 GC4: obj_and[4] = AND ( C8 , C9 )
 GC5: obj_and[5] = AND ( C10 , C11 )
 GC6: obj_and[6] = AND ( C12 , C13 )
End

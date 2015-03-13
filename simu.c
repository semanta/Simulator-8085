#include "simu.h"

const char * reg_map = "ABCDEHL", * flags_map = "S Z * AC * P * CY";

// get address value stored in HL register pair
int GETHLADDRESS(Simu * sim)  {
  return GETADDRESS(sim->REGISTERS[GETREGCHAR('H')], sim->REGISTERS[GETREGCHAR('L')]);
}
// get address value stored in HL register pair
int GETBCADDRESS(Simu * sim)  {
  return GETADDRESS(sim->REGISTERS[GETREGCHAR('B')], sim->REGISTERS[GETREGCHAR('C')]);
}
// get address value stored in HL register pair
int GETDEADDRESS(Simu * sim)  {
  return GETADDRESS(sim->REGISTERS[GETREGCHAR('D')], sim->REGISTERS[GETREGCHAR('E')]);
}
void MVI(Simu * simu, int opcode)
{
  //get the operand (immediate value)
  int data = simu->MEMORY[simu->PC++];
  //printf("value:%x\n",data);
  int D = getdest(opcode);
  if(ISMEM(D))
    { //MVI M, DATA
      simu->MEMORY[GETHLADDRESS(simu)] = data;
    }
  else
    { //MVI REG, DATA
      simu->REGISTERS[regidx(D)] = data;
    }
}

void MOV(Simu * sim, int opcode) {
    int S = getsource(opcode), D = getdest(opcode);
    ///EITHER S IS MEMORY OR D IS  MEMORY BUT NOT BOTH
    if (ISMEM(S)) {///SOURCE = MEMORY, MOV R, M
        sim->REGISTERS[regidx(D)] = sim->MEMORY[GETHLADDRESS(sim)];
    } else if (ISMEM(D)) { ///DEST = MEM, MOV M, R
        sim->MEMORY[GETHLADDRESS(sim)] = sim->REGISTERS[regidx(S)];
    } else { ///SOURCE = REGISTER, DESTINATION = REGISTER
        sim->REGISTERS[regidx(D)] = sim->REGISTERS[regidx(S)];
    }
}
void LXI(Simu * sim, int opcode) {
  int LB = sim->MEMORY[sim->PC++];
  int MB = sim->MEMORY[sim->PC++];
  int v = GETRP(opcode);
  if (v == 0) { ///BC
    sim->REGISTERS[GETREGCHAR('B')] = MB;
    sim->REGISTERS[GETREGCHAR('C')] = LB;
  } else if (v == 1) { ///DE
    sim->REGISTERS[GETREGCHAR('D')] = MB;
    sim->REGISTERS[GETREGCHAR('E')] = LB;
  } else if (v == 2) { ///HL
    sim->REGISTERS[GETREGCHAR('H')] = MB;
    sim->REGISTERS[GETREGCHAR('L')] = LB;
  } else if (v == 3) { ///sim->SP
    sim->SP = GETADDRESS(MB, LB);
  }
}


void SWAP_REG(Simu * sim, char a, char b)  {
  int t = sim->REGISTERS[GETREGCHAR(a)];
  sim->REGISTERS[GETREGCHAR(a)] = sim->REGISTERS[GETREGCHAR(b)];
  sim->REGISTERS[GETREGCHAR(b)] = t;
}
int getzero(Simu * sim) {
  return (sim->REGISTERS[7] >> 6) & 1;
}
int getcarry(Simu * sim) {
  return (sim->REGISTERS[7] >> 0) & 1;
}
int getsign(Simu * sim) {
  return (sim->REGISTERS[7] >> 7) & 1;
}
int getparity(Simu * sim) {
  return (sim->REGISTERS[7] >> 2) & 1;
}
int getAF(Simu * sim) {
  return (sim->REGISTERS[7] >> 4) & 1;
}

void setzero(Simu * sim) {
  sim->REGISTERS[7] |= (1 << 6);
}
void resetzero(Simu * sim) {
  sim->REGISTERS[7] &= ~(1 << 6);
}
void setcarry(Simu * sim) {
  sim->REGISTERS[7] |= (1 << 0);
}
void resetcarry(Simu * sim) {
  sim->REGISTERS[7] &= ~(1 << 0);
}
void setsign(Simu * sim) {
  sim->REGISTERS[7] |= (1 << 7);
}
void resetsign(Simu * sim) {
  sim->REGISTERS[7] &= ~(1 << 7);
}
void setparity(Simu * sim) {
  sim->REGISTERS[7] |= (1 << 2);
}
void resetparity(Simu * sim) {
  sim->REGISTERS[7] &= ~(1 << 2);
}
void setAF(Simu * sim) {
  sim->REGISTERS[7] |= (1 << 4);
}
void resetAF(Simu * sim) {
  sim->REGISTERS[7] &= ~(1 << 4);
}

void complement_carry(Simu * sim) {
  sim->REGISTERS[7] ^= 1 << 0;
}
///DOESNOT AFFECT AF FLAG, AFFECTS CARRY ACCORDING TO ecarry
void updateflags(Simu * sim, int v, int ecarry) {
  if ((v & 0xFF) == 0) setzero(sim);
  else resetzero(sim);
  if (ecarry) {
    if (v > 0xFF) setcarry(sim);
    else resetcarry(sim);
  }
  if ((v >> 7) & 1) setsign(sim);///CHECK IF MSB IS 1
  else resetsign(sim);
  if (popcount(v & 0xFF) & 1) resetparity(sim);
  else setparity(sim);
}
void DAD(Simu * sim, int v) {
  int reg_pair = GETRP(v);
  int op1 = GETHLADDRESS(sim);
  int op2 = 0;
  if (reg_pair == 0) op2 = GETBCADDRESS(sim);
  else if (reg_pair == 1) op2 = GETDEADDRESS(sim);
  else if (reg_pair == 2) op2 = op1; ///hl pair with hl pair addition = shift left
  else if (reg_pair == 3) op2 = sim->SP;
  op1 &= 0xFFFF;
  op2 &= 0xFFFF;
  int res = op1 + op2;
  if (res > 0xFFFF) setcarry(sim);
  else resetcarry(sim);
  sim->REGISTERS[GETREGCHAR('H')] = (res >> 8) & 0Xff;
  sim->REGISTERS[GETREGCHAR('L')] = (res >> 0) & 0Xff;
}
///with or without carry
void ADD(Simu * sim, int v) {
  int with_carry = (v >> 3) & 1;
  int op1 = sim->REGISTERS[GETREGCHAR('A')] & 0xFF;
  int op2;
  int id = getsource(v);
  if (ISMEM(id)) {
    op2 = sim->MEMORY[GETHLADDRESS(sim)] & 0xFF;
  } else {
    op2 = sim->REGISTERS[regidx(id)] & 0xFF;
  }
  int res = op1 + op2;
  int prevcarry = getcarry(sim);
  if (with_carry) res += prevcarry;
  sim->REGISTERS[GETREGCHAR('A')] = res & 0xFF;
  updateflags(sim, res, 1);
  int nib = (op1 & 0xF) + (op2 & 0xF);
  if (with_carry) nib += prevcarry;
  if (nib > 0xF) setAF(sim);
  else resetAF(sim);
}

///with or without carry
void ADI(Simu * sim, int v) {
  int with_carry = (v >> 3) & 1;
  int op1 = sim->REGISTERS[GETREGCHAR('A')] & 0xFF;
  int op2 = sim->MEMORY[sim->PC++] & 0xFF;;
  int res = op1 + op2;
  int prevcarry = getcarry(sim);
  if (with_carry) res += prevcarry;
  sim->REGISTERS[GETREGCHAR('A')] = res & 0xFF;
  updateflags(sim, res, 1);
  int nib = (op1 & 0xF) + (op2 & 0xF);
  if (with_carry) nib += prevcarry;
  if (nib > 0xF) setAF(sim);
  else resetAF(sim);
}
/// does not affect carry flag
void INR(Simu * sim, int v) {
  int dest = getdest(v);
  int * target = ISMEM(dest) ? &sim->MEMORY[GETHLADDRESS(sim)] : &sim->REGISTERS[regidx(dest)];
  int op = *target;
  ++*target;
  *target &= 0xFF;
  updateflags(sim, *target, 0);
  if ((op & 0xF) + 1 > 0xF) setAF(sim);
  else resetAF(sim);
}

void SETREGPAIR(Simu * sim, int reg_pair, int v) {
  int MB = (v >> 8) & 0xFF, LB = v & 0xFF;
  if (reg_pair == 0) {
    sim->REGISTERS[GETREGCHAR('B')] = MB;
    sim->REGISTERS[GETREGCHAR('C')] = LB;
  } else if (reg_pair == 1) {
    sim->REGISTERS[GETREGCHAR('D')] = MB;
    sim->REGISTERS[GETREGCHAR('E')] = LB;
  } else if (reg_pair == 2) {
    sim->REGISTERS[GETREGCHAR('H')] = MB;
    sim->REGISTERS[GETREGCHAR('L')] = LB;
  } else if (reg_pair == 3) sim->SP = v & 0xFFFF;
}

/// does not affect any flags
void INX(Simu * sim, int v) {
  int reg_pair = GETRP(v);
  int op = 0;
  if (reg_pair == 0) op = GETBCADDRESS(sim);
  else if (reg_pair == 1) op = GETDEADDRESS(sim);
  else if (reg_pair == 2) op = GETHLADDRESS(sim);
  else if (reg_pair == 3) op = sim->SP;
  op = (op + 1) & 0xFFFF;
  SETREGPAIR(sim, reg_pair, op);
}

/// does not affect any flags
void DCX(Simu * sim, int v) {
  int reg_pair = GETRP(v);
  int op = 0;
  if (reg_pair == 0) op = GETBCADDRESS(sim);
  else if (reg_pair == 1) op = GETDEADDRESS(sim);
  else if (reg_pair == 2) op = GETHLADDRESS(sim);
  else if (reg_pair == 3) op = sim->SP;
  op = (op - 1) & 0xFFFF;
  SETREGPAIR(sim, reg_pair, op);
}


/// does not affect carry flag
void DCR(Simu * sim, int v) {
  int dest = getdest(v);
  int * target = ISMEM(dest) ? &sim->MEMORY[GETHLADDRESS(sim)] : &sim->REGISTERS[regidx(dest)];
  int op = *target;
  *target += b8twos(1); ///SUBTRACT -1 = ADD TWOS complement OF 1
  *target &= 0xFF;
  updateflags(sim, *target, 0);
  if ((op & 0xF) + b4twos(1) > 0xF) resetAF(sim); ///borrow from the higher nibble to lower nibble
  else setAF(sim);
}

///with or without carry
void SUB(Simu * sim, int v) {
  int with_carry = (v >> 3) & 1;
  int op1 = sim->REGISTERS[GETREGCHAR('A')] & 0xFF;
  int op2;
  int id = getsource(v);
  if (ISMEM(id)) {
    op2 = sim->MEMORY[GETHLADDRESS(sim)] & 0xFF;
  } else {
    op2 = sim->REGISTERS[regidx(id)] & 0xFF;
  }
  int prevcarry = getcarry(sim);
  if (with_carry) op2 += prevcarry;
  int res = op1 + b8twos(op2);
  sim->REGISTERS[GETREGCHAR('A')] = res & 0xFF;
  updateflags(sim, res, 1);
  op2 &= 0xF;
  if (with_carry) op2 += prevcarry;
  int nib = (op1 & 0xF) + b4twos(op2 & 0xF);
  if (nib > 0xF) resetAF(sim); ///now borrow from the higher nibble
  else setAF(sim); ///borrow
  complement_carry(sim);
}

///with or without borrow
void SUI(Simu * sim, int v) {
  int with_carry = (v >> 3) & 1;
  int op1 = sim->REGISTERS[GETREGCHAR('A')] & 0xFF;
  int op2 = sim->MEMORY[sim->PC++] & 0xFF;;
  int prevcarry = getcarry(sim);
  if (with_carry) op2 += prevcarry;
  int res = op1 + b8twos(op2);
  sim->REGISTERS[GETREGCHAR('A')] = res & 0xFF;
  updateflags(sim, res, 1);
  op2 &= 0xF;
  if (with_carry) op2 += prevcarry;
  int nib = (op1 & 0xF) + b4twos(op2 & 0xF);
  if (nib > 0xF) resetAF(sim); ///now borrow from the higher nibble
  else setAF(sim); ///borrow
  complement_carry(sim);
}

void DAA(Simu * sim) {
  int * accu = &sim->REGISTERS[GETREGCHAR('A')];
  *accu &= 0xFF;
  if (((*accu & 0xF) > 0x9) || getAF(sim)) {
    *accu += 0x06;
    setAF(sim); ///CARRY TO HIGHER NIBBLE
  } else resetAF(sim);
  if ((((*accu >> 4) & 0xF) > 0x9) || getcarry(sim)) {
    *accu += 0x60;
    setcarry(sim);
  } else resetcarry(sim);
  updateflags(sim, (*accu) & 0xFF, 0);
}

void XCHG(Simu * sim) {
  SWAP_REG(sim, 'D', 'H');
  SWAP_REG(sim, 'E', 'L');
}
void LHLD(Simu * sim) {
  int LB = sim->MEMORY[sim->PC++];
  int MB = sim->MEMORY[sim->PC++];
  int EA = GETADDRESS(MB, LB);
  sim->REGISTERS[GETREGCHAR('L')] = sim->MEMORY[EA];
  EA++;
  sim->REGISTERS[GETREGCHAR('H')] = sim->MEMORY[EA];
}
void SHLD(Simu * sim) {
  int LB = sim->MEMORY[sim->PC++];
  int MB = sim->MEMORY[sim->PC++];
  int EA = GETADDRESS(MB, LB);
  sim->MEMORY[EA] = sim->REGISTERS[GETREGCHAR('L')];
  EA++;
  sim->MEMORY[EA] = sim->REGISTERS[GETREGCHAR('H')];
}
void LDA(Simu * sim) {
  int LB = sim->MEMORY[sim->PC++];
  int MB = sim->MEMORY[sim->PC++];
  sim->REGISTERS[GETREGCHAR('A')] = sim->MEMORY[GETADDRESS(MB, LB)];
}
void STA(Simu * sim) {
  int LB = sim->MEMORY[sim->PC++];
  int MB = sim->MEMORY[sim->PC++];
  sim->MEMORY[GETADDRESS(MB, LB)] = sim->REGISTERS[GETREGCHAR('A')];
}
void LDAXB(Simu * sim) {
  sim->REGISTERS[GETREGCHAR('A')] = sim->MEMORY[GETBCADDRESS(sim)];
}
void LDAXD(Simu * sim) {
  sim->REGISTERS[GETREGCHAR('A')] = sim->MEMORY[GETDEADDRESS(sim)];
}
void STAXB(Simu * sim) {
  sim->MEMORY[GETBCADDRESS(sim)] = sim->REGISTERS[GETREGCHAR('A')];
}
void STAXD(Simu * sim) {
  sim->MEMORY[GETDEADDRESS(sim)] = sim->REGISTERS[GETREGCHAR('A')];
}
void ANA(Simu * sim, int v) {
  int id = getsource(v);
  int with;
  if (ISMEM(id)) {
    with = sim->MEMORY[GETHLADDRESS(sim)] & 0xFF;
  } else {
    with = sim->REGISTERS[regidx(id)] & 0xFF;
  }
  sim->REGISTERS[GETREGCHAR('A')] &= 0xFF;
  sim->REGISTERS[GETREGCHAR('A')] &= with;
  setAF(sim);
  resetcarry(sim);
  updateflags(sim, sim->REGISTERS[GETREGCHAR('A')], 0);
}
void ANI(Simu * sim) {
  int with = sim->MEMORY[sim->PC++] & 0xFF;
  sim->REGISTERS[GETREGCHAR('A')] &= 0xFF;
  sim->REGISTERS[GETREGCHAR('A')] &= with;
  setAF(sim);
  resetcarry(sim);
  updateflags(sim, sim->REGISTERS[GETREGCHAR('A')], 0);
}
void ORA(Simu * sim, int v) {
  int id = getsource(v);
  int with;
  if (ISMEM(id)) {
    with = sim->MEMORY[GETHLADDRESS(sim)] & 0xFF;
  } else {
    with = sim->REGISTERS[regidx(id)] & 0xFF;
  }
  sim->REGISTERS[GETREGCHAR('A')] &= 0xFF;
  sim->REGISTERS[GETREGCHAR('A')] |= with;
  resetAF(sim);
  resetcarry(sim);
  updateflags(sim, sim->REGISTERS[GETREGCHAR('A')], 0);
}
void ORI(Simu * sim) {
  int with = sim->MEMORY[sim->PC++] & 0xFF;
  sim->REGISTERS[GETREGCHAR('A')] &= 0xFF;
  sim->REGISTERS[GETREGCHAR('A')] |= with;
  resetAF(sim);
  resetcarry(sim);
  updateflags(sim, sim->REGISTERS[GETREGCHAR('A')], 0);
}
void XRA(Simu * sim, int v) {
  int id = getsource(v);
  int with;
  if (ISMEM(id)) {
    with = sim->MEMORY[GETHLADDRESS(sim)] & 0xFF;
  } else {
    with = sim->REGISTERS[regidx(id)] & 0xFF;
  }
  sim->REGISTERS[GETREGCHAR('A')] &= 0xFF;
  sim->REGISTERS[GETREGCHAR('A')] ^= with;
  resetAF(sim);
  resetcarry(sim);
  updateflags(sim, sim->REGISTERS[GETREGCHAR('A')], 0);
}
void XRI(Simu * sim) {
  int with = sim->MEMORY[sim->PC++] & 0xFF;
  sim->REGISTERS[GETREGCHAR('A')] &= 0xFF;
  sim->REGISTERS[GETREGCHAR('A')] ^= with;
  resetAF(sim);
  resetcarry(sim);
  updateflags(sim, sim->REGISTERS[GETREGCHAR('A')], 0);
}
void CMA(Simu * sim) {
  sim->REGISTERS[GETREGCHAR('A')] = (~sim->REGISTERS[GETREGCHAR('A')]) & 0xFF;
}
void CMC(Simu * sim) {
  sim->REGISTERS[7] ^= 1;
}
void STC(Simu * sim) {
  setcarry(sim);
}

///with or without carry
void RAL(Simu * sim, int v) {
  int with_carry = (v >> 4) & 1;
  int * accu = &sim->REGISTERS[GETREGCHAR('A')];
  int b = ((*accu) >> 7) & 1;
  *accu <<= 1;
  *accu &= 0xff;
  *accu &= ~1;
  *accu |= with_carry ? getcarry(sim) : b;
  if (b) setcarry(sim);
  else resetcarry(sim);
}

///with or without carry
void RAR(Simu * sim, int v) {
  int with_carry = (v >> 4) & 1;
  int * accu = &sim->REGISTERS[GETREGCHAR('A')];
  int b = (*accu >> 0) & 1;
  *accu &= 0xff;
  *accu >>= 1;
  *accu |= (with_carry ? getcarry(sim) : b) << 7;
  if (b) setcarry(sim);
  else resetcarry(sim);
}

void CMP(Simu * sim, int v) {
  int id = getsource(v);
  int op2;
  if (ISMEM(id)) {
    op2 = sim->MEMORY[GETHLADDRESS(sim)] & 0xFF;
  } else {
    op2 = sim->REGISTERS[regidx(id)] & 0xFF;
  }
  sim->REGISTERS[GETREGCHAR('A')] &= 0xFF;
  int op1 = sim->REGISTERS[GETREGCHAR('A')];
  int d = op1 + b8twos(op2);
  updateflags(sim, d, 1);
  int nib = (op1 & 0xF) + b4twos(op2 & 0xF);
  if (nib > 0xF) resetAF(sim);
  else setAF(sim);
  complement_carry(sim);
}

void CPI(Simu * sim) {
  int op2 = sim->MEMORY[sim->PC++] & 0xFF;
  sim->REGISTERS[GETREGCHAR('A')] &= 0xFF;
  int op1 = sim->REGISTERS[GETREGCHAR('A')];
  int d = op1 + b8twos(op2);
  updateflags(sim, d, 1);
  int nib = (op1 & 0xF) + b4twos(op2 & 0xF);
  if (nib > 0xF) resetAF(sim);
  else setAF(sim);
  complement_carry(sim);
}

void BRANCH(Simu * sim, int call) {
  int LB = sim->MEMORY[sim->PC++] & 0xFF;
  int MB = sim->MEMORY[sim->PC++] & 0xFF;
  if (call) {
    assert(sim->SP > 1);
    sim->MEMORY[--sim->SP] = (sim->PC >> 8) & 0xFF;
    sim->MEMORY[--sim->SP] = (sim->PC >> 0) & 0xFF;
  }
  sim->PC = GETADDRESS(MB, LB);
}

void BRANCH_ZERO(Simu * sim, int v, int call) {
  int LB = sim->MEMORY[sim->PC++] & 0xFF;
  int MB = sim->MEMORY[sim->PC++] & 0xFF;
  if (getzero(sim) == v) {
    if (call) {
      assert(sim->SP > 1);
      sim->MEMORY[--sim->SP] = (sim->PC >> 8) & 0xFF;
      sim->MEMORY[--sim->SP] = (sim->PC >> 0) & 0xFF;
    }
    sim->PC = GETADDRESS(MB, LB);
  }
}

void BRANCH_PARITY(Simu * sim, int v, int call) {
  int LB = sim->MEMORY[sim->PC++] & 0xFF;
  int MB = sim->MEMORY[sim->PC++] & 0xFF;
  if (getparity(sim) == v) {
    if (call) {
      assert(sim->SP > 1);
      sim->MEMORY[--sim->SP] = (sim->PC >> 8) & 0xFF;
      sim->MEMORY[--sim->SP] = (sim->PC >> 0) & 0xFF;
    }
    sim->PC = GETADDRESS(MB, LB);
  }
}

void BRANCH_CARRY(Simu * sim, int v, int call) {
  int LB = sim->MEMORY[sim->PC++] & 0xFF;
  int MB = sim->MEMORY[sim->PC++] & 0xFF;
  if (getcarry(sim) == v) {
    if (call) {
      assert(sim->SP > 1);
      sim->MEMORY[--sim->SP] = (sim->PC >> 8) & 0xFF;
      sim->MEMORY[--sim->SP] = (sim->PC >> 0) & 0xFF;
    }
    sim->PC = GETADDRESS(MB, LB);
  }
}

void BRANCH_SIGN(Simu * sim, int v, int call) {
  int LB = sim->MEMORY[sim->PC++] & 0xFF;
  int MB = sim->MEMORY[sim->PC++] & 0xFF;
  if (getsign(sim) == v) {
    if (call) {
      assert(sim->SP > 1);
      sim->MEMORY[--sim->SP] = (sim->PC >> 8) & 0xFF;
      sim->MEMORY[--sim->SP] = (sim->PC >> 0) & 0xFF;
    }
    sim->PC = GETADDRESS(MB, LB);
  }
}
void RET(Simu * sim) {
  assert(sim->SP <= 0xFFFF - 2);
  int LB = sim->MEMORY[sim->SP++] & 0xFF;
  int MB = sim->MEMORY[sim->SP++] & 0xFF;
  sim->PC = GETADDRESS(MB, LB);
}
void RET_SIGN(Simu * sim, int v) {
  if (getsign(sim) == v) {
    assert(sim->SP <= 0xFFFF - 2);
    int LB = sim->MEMORY[sim->SP++] & 0xFF;
    int MB = sim->MEMORY[sim->SP++] & 0xFF;
    sim->PC = GETADDRESS(MB, LB);
  }
}
void RET_CARRY(Simu * sim, int v) {
  if (getcarry(sim) == v) {
    assert(sim->SP <= 0xFFFF - 2);
    int LB = sim->MEMORY[sim->SP++] & 0xFF;
    int MB = sim->MEMORY[sim->SP++] & 0xFF;
    sim->PC = GETADDRESS(MB, LB);
  }
}

void PUSHB(Simu * sim) {
  assert(sim->SP > 1);
  sim->MEMORY[--sim->SP] = sim->REGISTERS[GETREGCHAR('B')];
  sim->MEMORY[--sim->SP] = sim->REGISTERS[GETREGCHAR('C')];
}
void PUSHD(Simu * sim) {
  assert(sim->SP > 1);
  sim->MEMORY[--sim->SP] = sim->REGISTERS[GETREGCHAR('D')];
  sim->MEMORY[--sim->SP] = sim->REGISTERS[GETREGCHAR('E')];
}
void PUSHH(Simu * sim) {
  assert(sim->SP > 1);
  sim->MEMORY[--sim->SP] = sim->REGISTERS[GETREGCHAR('H')];
  sim->MEMORY[--sim->SP] = sim->REGISTERS[GETREGCHAR('L')];
}
void PUSHPSW(Simu * sim) {
  assert(sim->SP > 1);
  sim->MEMORY[--sim->SP] = sim->REGISTERS[GETREGCHAR('A')];
  sim->MEMORY[--sim->SP] = sim->REGISTERS[7];
}
void POPPSW(Simu * sim) {
  assert(sim->SP <= 0xFFFF - 2);
  sim->REGISTERS[7] = sim->MEMORY[sim->SP++];
  sim->REGISTERS[GETREGCHAR('A')] = sim->MEMORY[sim->SP++];
}
void POPB(Simu * sim) {
  assert(sim->SP <= 0xFFFF - 2);
  sim->REGISTERS[GETREGCHAR('C')] = sim->MEMORY[sim->SP++];
  sim->REGISTERS[GETREGCHAR('B')] = sim->MEMORY[sim->SP++];
}
void POPD(Simu * sim) {
  assert(sim->SP <= 0xFFFF - 2);
  sim->REGISTERS[GETREGCHAR('E')] = sim->MEMORY[sim->SP++];
  sim->REGISTERS[GETREGCHAR('D')] = sim->MEMORY[sim->SP++];
}

void POPH(Simu * sim) {
  assert(sim->SP <= 0xFFFF - 2);
  sim->REGISTERS[GETREGCHAR('L')] = sim->MEMORY[sim->SP++];
  sim->REGISTERS[GETREGCHAR('H')] = sim->MEMORY[sim->SP++];
}
void SPHL(Simu * sim) {
  sim->SP = GETHLADDRESS(sim);
}
void PCHL(Simu * sim) {
  sim->PC = GETHLADDRESS(sim);
}

void XTHL(Simu * sim) {
  assert(sim->SP <= 0XFFFF - 2); ///stack shouldn't be empty
  int * LB = &sim->MEMORY[sim->SP], * MB = &sim->MEMORY[sim->SP + 1];
  int * H = &sim->REGISTERS[GETREGCHAR('H')];
  int * L = &sim->REGISTERS[GETREGCHAR('L')];
  *H ^= *MB ^= *H ^= *MB;
  *L ^= *LB ^= *L ^= *LB;
}


void RET_PARITY(Simu * sim, int v) {
  if (getparity(sim) == v) {
    assert(sim->SP <= 0xFFFF - 2);
    int LB = sim->MEMORY[sim->SP++] & 0xFF;
    int MB = sim->MEMORY[sim->SP++] & 0xFF;
    sim->PC = GETADDRESS(MB, LB);
  }
}

void RET_ZERO(Simu * sim, int v) {
  if (getzero(sim) == v) {
    assert(sim->SP <= 0xFFFF - 2);
    int LB = sim->MEMORY[sim->SP++] & 0xFF;
    int MB = sim->MEMORY[sim->SP++] & 0xFF;
    sim->PC = GETADDRESS(MB, LB);
  }
}

void IN(Simu * sim) {
  sim->REGISTERS[GETREGCHAR('A')] = sim->IOPORTS[sim->MEMORY[sim->PC++] & 0XFF];
}
void OUT(Simu * sim) {
  sim->IOPORTS[sim->MEMORY[sim->PC++] & 0XFF] = sim->REGISTERS[GETREGCHAR('A')];
}
void execute(Simu * simu)
{
  printf("here we execute\n");
  int i;
  while(simu->HALT!=1)
    {
      int v = simu->MEMORY[simu->PC++] & 0xFF;
      printf("pc:%d, :: %x\n",simu->PC,v);
      // NOP instruction
      if(v == 0) continue ;
      // halt hlt instruction
      if(is_hlt(v)) simu->HALT = 1;
      ///data transfer
      else if (is_mov(v)) MOV(simu, v);
      else if (is_mvi(v)) MVI(simu, v);
      else if (is_lxi(v)) LXI(simu, v);
      else if (is_lda(v)) LDA(simu);
      else if (is_sta(v)) STA(simu);
      else if (is_lhld(v)) LHLD(simu);
      else if (is_shld(v)) SHLD(simu);
      else if (is_ldaxb(v)) LDAXB(simu);
      else if (is_ldaxd(v)) LDAXD(simu);
      else if (is_staxb(v)) STAXB(simu);
      else if (is_staxd(v)) STAXD(simu);
      else if (is_xchg(v)) XCHG(simu);
      ///arithmetic
    else if (is_add(v)) ADD(simu, v);
    else if (is_adi(v)) ADI(simu, v);
    else if (is_sub(v)) SUB(simu, v);
    else if (is_sbi(v)) SUI(simu, v);
    else if (is_dad(v)) DAD(simu, v);
    else if (is_inr(v)) INR(simu, v);
    else if (is_dcr(v)) DCR(simu, v);
    else if (is_inx(v)) INX(simu, v);
    else if (is_dcx(v)) DCX(simu, v);
    else if (is_daa(v)) DAA(simu);
    ///logical
    else if (is_ana(v)) ANA(simu, v);
    else if (is_ani(v)) ANI(simu);
    else if (is_ora(v)) ORA(simu, v);
    else if (is_ori(v)) ORI(simu);
    else if (is_xra(v)) XRA(simu, v);
    else if (is_xri(v)) XRI(simu);
    else if (is_cma(v)) CMA(simu);
    else if (is_cmc(v)) CMC(simu);
    else if (is_stc(v)) STC(simu);
    else if (is_cmp(v)) CMP(simu, v);
    else if (is_cpi(v)) CPI(simu);
    else if (is_rotleft(v)) RAL(simu, v);
    else if (is_rotright(v)) RAR(simu, v);
    ///branching
    else if (is_jmp(v)) BRANCH(simu, 0);
    else if (is_jnz(v)) BRANCH_ZERO(simu, 0, 0);
    else if (is_jz(v)) BRANCH_ZERO(simu, 1, 0);
    else if (is_jnc(v)) BRANCH_CARRY(simu, 0, 0);
    else if (is_jc(v)) BRANCH_CARRY(simu, 1, 0);
    else if (is_jpe(v)) BRANCH_PARITY(simu, 1, 0);
    else if (is_jpo(v)) BRANCH_PARITY(simu, 0, 0);
    else if (is_jp(v)) BRANCH_SIGN(simu, 0, 0);
    else if (is_jm(v)) BRANCH_SIGN(simu, 1, 0);
    else if (is_call(v)) BRANCH(simu, 1);
    else if (is_cnz(v)) BRANCH_ZERO(simu, 0, 1);
    else if (is_cz(v)) BRANCH_ZERO(simu, 1, 1);
    else if (is_cnc(v)) BRANCH_CARRY(simu, 0, 1);
    else if (is_cc(v)) BRANCH_CARRY(simu, 1, 1);
    else if (is_cpe(v)) BRANCH_PARITY(simu, 1, 1);
    else if (is_cpo(v)) BRANCH_PARITY(simu, 0, 1);
    else if (is_cp(v)) BRANCH_SIGN(simu, 0, 1);
    else if (is_cm(v)) BRANCH_SIGN(simu, 1, 1);
    else if (is_ret(v)) RET(simu);
    else if (is_rnz(v)) RET_ZERO(simu, 0);
    else if (is_rz(v)) RET_ZERO(simu, 1);
    else if (is_rnc(v)) RET_CARRY(simu, 0);
    else if (is_rc(v)) RET_CARRY(simu, 1);
    else if (is_rpe(v)) RET_PARITY(simu, 1);
    else if (is_rpo(v)) RET_PARITY(simu, 0);
    else if (is_rp(v)) RET_SIGN(simu, 0);
    else if (is_rm(v)) RET_SIGN(simu, 1);
    ///stack + misc
    else if (is_pushb(v)) PUSHB(simu);
    else if (is_pushd(v)) PUSHD(simu);
    else if (is_pushh(v)) PUSHH(simu);
    else if (is_pushpsw(v)) PUSHPSW(simu);
    else if (is_popb(v)) POPB(simu);
    else if (is_popd(v)) POPD(simu);
    else if (is_poph(v)) POPH(simu);
    else if (is_poppsw(v)) POPPSW(simu);
    else if (is_sphl(v)) SPHL(simu);
    else if (is_xthl(v)) XTHL(simu);
    else if (is_pchl(v)) PCHL(simu);
    else if (is_in(v)) IN(simu);
    else if (is_out(v)) OUT(simu);

    }
}

void loadprogram(Simu * simu, int start_addr, FileToLines * progFile)
{
  int i;
  simu->PC = simu->START_ADDRESS = start_addr;
  // stack pointer points at FFFF location
  simu->SP =  0XFFFF;
  simu->progFile.lineCount = 0;
  simu->errorList.count = 0;
  simu->labelList.count = 0;
  simu->byteList.count = 0;
  // parse and store in memory
  parseFile(progFile, &simu->byteList, &simu->labelList, &simu->errorList,simu->START_ADDRESS);
  int ADDR = simu->PC;
  if(simu->errorList.count == 0)
    {
      for( i = 0; i < simu->byteList.count; ++i)
	{
	  simu->MEMORY[ADDR++] = simu->byteList.bytes[i][0];
	}
    }


}


void stat(Simu * sim) {
  printf("PC: %d, SP: %d, HALT: %d\n", sim->PC, sim->SP, sim->HALT);
    int i;
    for (i = 0; i < 7; ++i) {
        printf("%c: %x ", reg_map[i], sim->REGISTERS[i] & 0xFF);
    }
    putchar('\n');
    // printf("S: %d Z: %d AC: %d P: %d C: %d\n", getsign(sim), getzero(sim), getAF(sim), getparity(sim), getcarry(sim));
}

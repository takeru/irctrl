#include <IrCtrl.h>

int PIN_LED      = 13;
int PIN_IR_IN    =  8; // PB0 ICP1 Counter1
int PIN_IR_OUT   =  3; // PD3 OC2B Timer2

void setup()
{
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_IR_OUT, OUTPUT);
  pinMode(PIN_IR_IN,  INPUT);
  digitalWrite(PIN_IR_IN, HIGH); // pull-up
  IR_initialize();
  Serial.println("IrCtrlDemo");
}

void loop()
{
  int i;

  digitalWrite(PIN_LED, !digitalRead(PIN_IR_IN));
  ir_recv();

  if(0<Serial.available()){
    char c = Serial.read();
    if(c=='1'){
      // [AEHA] AA 5A 8F 12 14 F1
      if(IR_xmit(AEHA, (uint8_t*)"\xAA\x5A\x8F\x12\x14\xF1", 6*8)){
        Serial.println("[SENT] AQUOS VOLUME UP");
      }
    }else if(c=='3'){
      // [AEHA] 23 CB 26 01 00 20 18 0C 36 79 00 00 00 00 00 10 00 18
      if(IR_xmit(AEHA, (uint8_t*)"\x23\xCB\x26\x01\x00\x20\x18\x0C\x36\x79\x00\x00\x00\x00\x00\x10\x00\x18", 18*8)){
        Serial.println("[SENT] MITSUBISHI KIRIGAMINE COOL 28C ON");
      }
    }else if(c=='4'){
      // [AEHA] 23 CB 26 01 00 00 18 0C 36 79 00 00 00 00 00 10 00 F8
      if(IR_xmit(AEHA, (uint8_t*)"\x23\xCB\x26\x01\x00\x00\x18\x0C\x36\x79\x00\x00\x00\x00\x00\x10\x00\xF8", 18*8)){
        Serial.println("[SENT] MITSUBISHI KIRIGAMINE OFF");
      }
    }else if(c=='5'){
      // [SONY20] 15 2D 0F (D=15 A=1E5A)
      for(i=0; i<3; i++){
        if(IR_xmit(SONY, (uint8_t*)"\x15\x2D\x0F", 20)){
          Serial.println("[SENT] SONY RECOADER POWER");
        }
        delay(45UL);
      }
    }else if(c=='6'){
      // [SONY20] 42 2D 0F (D=42 A=1E5A)
      for(i=0; i<3; i++){
        if(IR_xmit(SONY, (uint8_t*)"\x42\x2D\x0F", 20)){
          Serial.println("[SENT] SONY RECOADER HOME");
        }
        delay(45UL);
      }
    }else if(c=='\r' || c=='\n'){
      // nop.
    }else{
      Serial.println("IrCtrlDemo: unknown command.");
    }
  }
}

void Serial_print_hex(unsigned char c){
  if(c<=0x0F){
    Serial.print(0, HEX);
  }
  Serial.print(c, HEX);
}

void ir_recv(void)
{
  if(IrCtrl.state!=IR_RECVED){
    return;
  }

  uint8_t d, i, l;
  uint16_t a;

  l = IrCtrl.len;
  switch (IrCtrl.fmt) {	/* Which frame arrived? */
#if IR_USE_NEC
  case NEC:	/* NEC format data frame */
    if (l == 32) {	/* Only 32-bit frame is valid */
      Serial.print("[NEC] ");
      Serial_print_hex(IrCtrl.buff[0]); Serial.print(" ");
      Serial_print_hex(IrCtrl.buff[1]); Serial.print(" ");
      Serial_print_hex(IrCtrl.buff[2]); Serial.print(" ");
      Serial_print_hex(IrCtrl.buff[3]); Serial.println();
      // TODO test it.
    }
    break;
  case NEC|REPT:	/* NEC repeat frame */
    Serial.println("[NEC repeat]");
    // TODO test it.
    break;
#endif
#if IR_USE_AEHA
  case AEHA:		/* AEHA format data frame */
    if ((l >= 48) && (l % 8 == 0)) {	/* Only multiple of 8 bit frame is valid */
      Serial.print("[AEHA]");
      l /= 8;
      for (i = 0; i < l; i++){
        Serial.print(" ");
        Serial_print_hex(IrCtrl.buff[i]);
      }
      Serial.println();
    }
    break;
  case AEHA|REPT:	/* AEHA format repeat frame */
    Serial.println("[AEHA repeat]");
    // TODO test it.
    break;
#endif
#if IR_USE_SONY
  case SONY:
    d = IrCtrl.buff[0];
    a = ((uint16_t)IrCtrl.buff[2] << 9) + ((uint16_t)IrCtrl.buff[1] << 1) + ((d & 0x80) ? 1 : 0);
    d &= 0x7F;
    switch (l) {	/* Only 12, 15 or 20 bit frames are valid */
    case 12:
      Serial.print("[SONY12] ");
      Serial_print_hex(d);        Serial.print(" ");
      Serial_print_hex(a & 0x1F); Serial.println();
      // TODO test it.
      break;
    case 15:
      Serial.print("[SONY15] ");
      Serial_print_hex(d);        Serial.print(" ");
      Serial_print_hex(a & 0xFF); Serial.println();
      // TODO test it.
      break;
    case 20:
      Serial.print("[SONY20] ");
      Serial_print_hex(IrCtrl.buff[0]       ); Serial.print(" ");
      Serial_print_hex(IrCtrl.buff[1]       ); Serial.print(" ");
      Serial_print_hex(IrCtrl.buff[2] & 0x0F); Serial.print(" (D=");
      Serial_print_hex(d);
      Serial.print(" A=");
      Serial_print_hex(a>>8 & 0x1F);
      Serial_print_hex(a    & 0xFF);
      Serial.println(")");
      break;
    }
    break;
#endif
  }
  IrCtrl.state = IR_IDLE;		/* Ready to receive next frame */
}


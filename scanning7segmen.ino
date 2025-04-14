uint8_t angka[10] = { 0x7e, 0x0c, 0xb6, 0x9e, 0xcc, 0xda, 0xfa, 0x0e, 0xfe, 0xde };
bool dot;
void display_segmen(int data) {
  //ribuan
  PORTB = 0x0e;
  PORTD = angka[(data / 1000) % 10];
  PORTB &= ~(1 << PB4);
  _delay_ms(5);
  //ratusan
  PORTB = 0x0d;
  PORTD = angka[(data / 100) % 10];
  PORTB = (dot) ?  (PORTB & ~(1 << PB4)): (PORTB | (1 << PB4));
  _delay_ms(5);
  //puluhan
  PORTB = 0x0b;
  PORTD = angka[(data / 10) % 10];
  PORTB &= ~(1 << PB4);
  _delay_ms(5);
  //satuan
  PORTB = 0x07;
  PORTD = angka[data % 10];
  PORTB &= ~(1 << PB4);
  _delay_ms(5);
}

int main() {
  DDRD = 0xfe;
  DDRB = 0x1f;
  DDRC &= ~(1 << PC0);
  DDRC |= (1 << PC2) | (1 << PC3) | (1 << PC4);
  PORTC |= (1 << PC0);
  int i = 0;
  int waktu = 0;
  bool pb, last_pb;
  bool buz = false;
  
  while (1) {
    pb = !(PINC & (1 << PC0));

    if (pb && !last_pb) {
      buz = true;
    }
    last_pb = pb;

    display_segmen(i);

    // if (buz) PORTC |= (1 << PC2);
    // else PORTC &= ~(1 << PC2);

    waktu++;
    if (waktu == 50) {
      waktu = 0;
      dot = !dot;
      if(!dot) i = (i+1)%100;
      // buz = false;
    }
  }
  return 0;
}
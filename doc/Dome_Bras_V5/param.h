#include <LiquidCrystal595.h>
#define Bouton 8
#define LCDwidth 16
#define LCDheight 2

class parameter_ {

  public:
    int _step;
    int _val;
    byte _min;
    int _max;
    String _type;
    String _name;

    parameter_(String type, int val, uint8_t step,  byte min, int max, String name) {
      _step = step;
      _type = type;
      _min = min;
      _max = max;
      _val = val;
      _name = name;
    }

    void afficher(LiquidCrystal595 lcd) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("<");
      lcd.print(centerElement(this->_name));
      lcd.setCursor(15, 0);
      lcd.print(">");
      lcd.setCursor(0, 1);
      lcd.print(centerElement(String(this->_val)));
    }

    void incr() {
      if (this->_type == "bool")
      {
        this->_val = !this->_val;
      }
      else
      {
        if (this->_val < this->_max)
        {
          this->_val = this->_val + this->_step;
        }
        else
        {
          this->_val = this->_max ;
        }
      }
    }

    void decr() {
      if (this->_type == "bool")
      {
        this->_val = !this->_val;
      }
      else
      {
        if (this->_val > this->_min)
        {
          this->_val = this->_val - this->_step;
        }
        else
        {
          this->_val = this->_min;
        }
      }
    }

    void ctrl(LiquidCrystal595 lcd, Rotary rotary)
    {
      boolean updatelcd = true;
      boolean _sortie = false;

      while (_sortie == false)
      {

        unsigned char result = rotary.process();
        bool boutonState = digitalRead(Bouton);


        if (updatelcd == true)
        {
          lcd.clear();
          lcd.setCursor(1, 0);
          lcd.print(centerElement(this->_name));
          lcd.setCursor(0, 1);
          lcd.print(centerElement(String(this->_val)));
          lcd.setCursor(0, 1);
          lcd.print("<");
          lcd.setCursor(15, 1);
          lcd.print(">");
          updatelcd = false;
        }
        if (result == DIR_CW) {
          this->incr();
          updatelcd = true;
        } else if (result == DIR_CCW) {
          this->decr();
          updatelcd = true;
        }
        if (boutonState == 0) {
          delay(250);
          _sortie = true;
          updatelcd = true;
        }
      }
    }


  private:
    String centerElement(String element) {
      int textLength = element.length();
      int paddingLeft;
      if (textLength < LCDwidth - 2) {
        int padding = LCDwidth - textLength;
        paddingLeft = padding / 2;
      }
      String ret = "";
      if (textLength % 2 == 0) {
        paddingLeft--;
      }
      for (int i = 0; i < paddingLeft; i++) {
        ret = ret + " ";
      }
      ret = ret + element;
      for (int i = 0; i < paddingLeft; i++) {
        ret = ret + " ";
      }
      return ret;
    }
};

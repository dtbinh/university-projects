CREATE OR REPLACE PROCEDURE PRC_INV_ADD (W_CUS_CODE IN VARCHAR2, W_DATE IN DATE)
AS BEGIN
   INSERT INTO INVOICE
          VALUES(INV_NUMBER_SEQ.NEXTVAL, W_CUS_CODE, W_DATE);
   DBMS_OUTPUT.PUT_LINE('Invoice added');
END;


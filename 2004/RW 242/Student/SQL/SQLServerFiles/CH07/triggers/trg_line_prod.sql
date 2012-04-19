IF EXISTS (SELECT name FROM sysobjects
      WHERE name = 'TRG_LINE_PROD' AND type = 'TR')
   DROP TRIGGER TRG_LINE_PROD
GO
CREATE TRIGGER TRG_LINE_PROD
ON LINE
FOR INSERT
AS
BEGIN
   UPDATE PRODUCT
     SET P_ONHAND = (SELECT PRODUCT.P_ONHAND-INSERTED.LINE_UNITS FROM PRODUCT, INSERTED
   	 WHERE PRODUCT.P_CODE = INSERTED.P_CODE)
END;

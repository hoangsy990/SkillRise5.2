-- Isolated QA-only test. Run with sqlcmd -b -E -S (local) -d GrowLancer_QA -i this-file.
SET NOCOUNT ON;

IF DB_NAME() <> 'GrowLancer_QA'
BEGIN
    RAISERROR('Refusing class-seed test outside GrowLancer_QA', 16, 1);
    RETURN;
END;

IF EXISTS (SELECT 1 FROM dbo.AccountCharacter WHERE Id = 'GLQA9Z01')
   OR EXISTS (SELECT 1 FROM dbo.Character WHERE Name = 'GLQA9ZC1')
BEGIN
    RAISERROR('Synthetic QA identity already exists; refusing overwrite', 16, 1);
    RETURN;
END;

IF EXISTS (SELECT 1 FROM dbo.DefaultClassType WHERE Class = 111)
BEGIN
    RAISERROR('Negative fixture class111 unexpectedly has a starter row', 16, 1);
    RETURN;
END;

DECLARE @result TABLE (Result tinyint);
BEGIN TRY
    BEGIN TRANSACTION;
    INSERT INTO @result EXEC dbo.WZ_CreateCharacter 'GLQA9Z01', 'GLQA9ZC1', 111;
    IF (SELECT COUNT(*) FROM @result WHERE Result = 2) <> 1
       OR EXISTS (SELECT 1 FROM dbo.Character WHERE Name = 'GLQA9ZC1')
       OR EXISTS (SELECT 1 FROM dbo.AccountCharacter WHERE Id = 'GLQA9Z01')
    BEGIN
        RAISERROR('Missing starter class111 was not rejected without writes', 16, 1);
    END;
    DELETE FROM @result;

    INSERT INTO @result EXEC dbo.WZ_CreateCharacter 'GLQA9Z01', 'GLQA9ZC1', 112;

    IF (SELECT COUNT(*) FROM @result WHERE Result = 1) <> 1
       OR (SELECT COUNT(*) FROM dbo.Character WHERE Name = 'GLQA9ZC1'
           AND Class = 112 AND Strength = 25 AND Dexterity = 27
           AND Vitality = 32 AND Energy = 20 AND MapNumber = 0
           AND MapPosX = 150 AND MapPosY = 126) <> 1
    BEGIN
        RAISERROR('QA class112 create/result/starter mismatch', 16, 1);
    END;

    ROLLBACK TRANSACTION;
END TRY
BEGIN CATCH
    IF @@TRANCOUNT > 0 ROLLBACK TRANSACTION;
    DECLARE @error nvarchar(4000) = ERROR_MESSAGE();
    RAISERROR('%s', 16, 1, @error);
    RETURN;
END CATCH;

IF EXISTS (SELECT 1 FROM dbo.AccountCharacter WHERE Id = 'GLQA9Z01')
   OR EXISTS (SELECT 1 FROM dbo.Character WHERE Name = 'GLQA9ZC1')
BEGIN
    RAISERROR('QA synthetic row persisted after rollback', 16, 1);
    RETURN;
END;

PRINT 'PASS isolated class111 rejection, class112 starter/result/rollback; synthetic rows 0/0';

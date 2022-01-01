function doGet(e) { 
    Logger.log( JSON.stringify(e) );
    var result = 'Ok';
    if (e.parameter == 'undefined') {
      result = 'No Parameters';
    }
    else {
      var sheet_id = '1aYy201GVkZix2j_0dEFz-DCC3SBRA98WIdT64kg1hXw'; 	// Spreadsheet ID
      var sheet = SpreadsheetApp.openById(sheet_id).getActiveSheet();
      var newRow = sheet.getLastRow() + 1;						
      var rowData = [];
      var Curr_Date = new Date();
      rowData[0] = Curr_Date; // Date in column A
      var Curr_Time = Utilities.formatDate(Curr_Date, "Asia/Jakarta", 'HH:mm:ss');
      rowData[1] = Curr_Time; // Time in column B
      for (var param in e.parameter) {
        Logger.log('In for loop, param=' + param);
        var value = stripQuotes(e.parameter[param]);
        Logger.log(param + ':' + e.parameter[param]);
        switch (param) {
          case 'nilai':
            rowData[2] = value+'°C'; // Temperature in column C
            result = 'Nilai Written on column C'; 
            break;
          case 'range':
            rowData[4] = "Testing";
            result = "range written on column E"
            break;
        }
      }
      Logger.log(JSON.stringify(rowData));
      var newRange = sheet.getRange(newRow, 1, 1, rowData.length);
      newRange.setValues([rowData]);
    }
    return ContentService.createTextOutput(result);
  }
  function stripQuotes( value ) {
    return value.replace(/^["']|['"]$/g, "");
  }
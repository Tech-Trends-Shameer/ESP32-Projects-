var ss = SpreadsheetApp.openById('Sheet Id');
var sheet = ss.getSheetByName('Sheet1');


function doGet(e){
  var read = e.parameter.read;

  if (read !== undefined){
    return ContentService.createTextOutput(sheet.getRange('A1').getValue());
  }
}

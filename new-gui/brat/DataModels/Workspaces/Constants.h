#if !defined(WORKSPACES_CONSTANTS_H)
#define WORKSPACES_CONSTANTS_H


const std::string WKS_EXPRESSION_NAME = "Expression";

const std::string GROUP_DATASETS = "Datasets";
const std::string GROUP_OPERATIONS = "Operations";
const std::string GROUP_FORMULAS = "Formulas";
const std::string GROUP_FUNCTIONS = "Functions";
const std::string GROUP_DISPLAY = "Displays";

const std::string ENTRY_WKSNAME = "WorkspaceName";
const std::string ENTRY_WKSLEVEL = "WorkspaceLevel";

const std::string ENTRY_RECORDNAME = "RecordName";
const std::string ENTRY_DSNAME = "DatasetName";
const std::string ENTRY_OPNAME = "OperationName";
const std::string ENTRY_FILE = "File";
const std::string ENTRY_FILE_REGEX = ENTRY_FILE + "\\d+";
const std::string ENTRY_UNIT = "Unit";
const std::string ENTRY_UNIT_REGEX = ENTRY_UNIT + "_(.+)";
const std::string ENTRY_DISPLAYNAME = "DisplayName";

const std::string ENTRY_DISPLAY_DATA = "DisplayData";
const std::string ENTRY_FORMULA = "Formula";
const std::string ENTRY_SELECT = "Select";

const std::string ENTRY_DEFINE = "Define";
const std::string ENTRY_COMMENT = "Comment";
const std::string ENTRY_TYPE = "Type";
const std::string ENTRY_OPERATION_FILTER = "v4Filter";
const std::string ENTRY_DATA_MODE = "DataMode";
const std::string ENTRY_FIELD = "Field";
const std::string ENTRY_FIELDNAME = "FieldName";
const std::string ENTRY_FIELDTITLE = "FieldTitle";
const std::string ENTRY_FIELDTYPE = "FieldType";
const std::string ENTRY_DATATYPE = "DataType";
const std::string ENTRY_TITLE = "Title";
const std::string ENTRY_INTERVAL = "Interval";
const std::string ENTRY_STEP = "Step";
const std::string ENTRY_FILTER = "Filter";
const std::string ENTRY_MINVALUE = "MinValue";
const std::string ENTRY_MAXVALUE = "MaxValue";
const std::string ENTRY_MINXVALUE = "MinXValue";
const std::string ENTRY_MAXXVALUE = "MaxXValue";
const std::string ENTRY_MINYVALUE = "MinYValue";
const std::string ENTRY_MAXYVALUE = "MaxYValue";
const std::string ENTRY_LOESSCUTOFF = "LoessCutOff";
const std::string ENTRY_ANIMATION = "Animation";
const std::string ENTRY_PROJECTION = "Projection";
const std::string ENTRY_GROUP = "Group";
const std::string ENTRY_CONTOUR = "Contour";
const std::string ENTRY_INVERT_XYAXES = "InvertXYAxes";
const std::string ENTRY_NUMBER_OF_BINS = "number_of_bins";		//v4
const std::string ENTRY_SOLID_COLOR = "SolidColor";
const std::string ENTRY_COLOR_PALETTE = "ColorPalette";
const std::string ENTRY_EAST_COMPONENT = "EastComponent";
const std::string ENTRY_NORTH_COMPONENT = "NorthComponent";

const std::string ENTRY_X = "X";
const std::string ENTRY_XDESCR = "XDescr";
const std::string ENTRY_XUNIT = "XUnit";
const std::string ENTRY_Y = "Y";
const std::string ENTRY_YDESCR = "YDescr";
const std::string ENTRY_YUNIT = "YUnit";
const std::string ENTRY_Z = "Z";
const std::string ENTRY_ZDESCR = "ZDescr";
const std::string ENTRY_ZUNIT = "ZUnit";

const std::string ENTRY_X_AXIS = "XAxis";
//const std::string ENTRY_X_AXIS_TEXT = "XAxisTitle";

const std::string ENTRY_OUTPUT = "Output";
const std::string ENTRY_EXPORT_ASCII_OUTPUT = "ExportAsciiOutput";
const std::string ENTRY_EXPORT_GEOTIFF_OUTPUT = "ExportGeoTIFFOutput";

const std::string ENTRY_ZOOM = "Zoom";



// v4

const std::string PROJECTION_3D_VALUE = "3D";



#endif      //WORKSPACES_CONSTANTS_H

#pragma once

#include "../ShellBrowser/iShellView.h"

static const int DEFAULT_COLUMN_WIDTH = 150;

static const Column_t REAL_FOLDER_DEFAULT_COLUMNS[] =
{{CM_NAME, TRUE, DEFAULT_COLUMN_WIDTH},
{CM_TYPE, TRUE, DEFAULT_COLUMN_WIDTH},
{CM_SIZE, TRUE, DEFAULT_COLUMN_WIDTH},
{CM_DATEMODIFIED, TRUE, DEFAULT_COLUMN_WIDTH},
{CM_ATTRIBUTES, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_REALSIZE, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_SHORTNAME, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_OWNER, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_PRODUCTNAME, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_COMPANY, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_DESCRIPTION, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_FILEVERSION, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_PRODUCTVERSION, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_SHORTCUTTO, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_HARDLINKS, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_EXTENSION, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_CREATED, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_ACCESSED, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_TITLE, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_SUBJECT, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_AUTHOR, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_KEYWORDS, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_COMMENT, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_CAMERAMODEL, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_DATETAKEN, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_WIDTH, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_HEIGHT, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_MEDIA_BITRATE, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_MEDIA_COPYRIGHT, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_MEDIA_DURATION, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_MEDIA_PROTECTED, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_MEDIA_RATING, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_MEDIA_ALBUMARTIST, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_MEDIA_ALBUM, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_MEDIA_BEATSPERMINUTE, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_MEDIA_COMPOSER, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_MEDIA_CONDUCTOR, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_MEDIA_DIRECTOR, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_MEDIA_GENRE, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_MEDIA_LANGUAGE, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_MEDIA_BROADCASTDATE, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_MEDIA_CHANNEL, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_MEDIA_STATIONNAME, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_MEDIA_MOOD, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_MEDIA_PARENTALRATING, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_MEDIA_PARENTALRATINGREASON, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_MEDIA_PERIOD, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_MEDIA_PRODUCER, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_MEDIA_PUBLISHER, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_MEDIA_WRITER, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_MEDIA_YEAR, FALSE, DEFAULT_COLUMN_WIDTH}};

static const Column_t CONTROL_PANEL_DEFAULT_COLUMNS[] =
{{CM_NAME, TRUE, DEFAULT_COLUMN_WIDTH},
{CM_VIRTUALCOMMENTS, TRUE, DEFAULT_COLUMN_WIDTH}};

static const Column_t MY_COMPUTER_DEFAULT_COLUMNS[] =
{{CM_NAME, TRUE, DEFAULT_COLUMN_WIDTH},
{CM_TYPE, TRUE, DEFAULT_COLUMN_WIDTH},
{CM_TOTALSIZE, TRUE, DEFAULT_COLUMN_WIDTH},
{CM_FREESPACE, TRUE, DEFAULT_COLUMN_WIDTH},
{CM_VIRTUALCOMMENTS, FALSE, DEFAULT_COLUMN_WIDTH},
{CM_FILESYSTEM, FALSE, DEFAULT_COLUMN_WIDTH}};

static const Column_t RECYCLE_BIN_DEFAULT_COLUMNS[] =
{{CM_NAME, TRUE, DEFAULT_COLUMN_WIDTH},
{CM_ORIGINALLOCATION, TRUE, DEFAULT_COLUMN_WIDTH},
{CM_DATEDELETED, TRUE, DEFAULT_COLUMN_WIDTH},
{CM_SIZE, TRUE, DEFAULT_COLUMN_WIDTH},
{CM_TYPE, TRUE, DEFAULT_COLUMN_WIDTH},
{CM_DATEMODIFIED, TRUE, DEFAULT_COLUMN_WIDTH}};

static const Column_t PRINTERS_DEFAULT_COLUMNS[] =
{{CM_NAME, TRUE, DEFAULT_COLUMN_WIDTH},
{CM_NUMPRINTERDOCUMENTS, TRUE, DEFAULT_COLUMN_WIDTH},
{CM_PRINTERSTATUS, TRUE, DEFAULT_COLUMN_WIDTH},
{CM_PRINTERCOMMENTS, TRUE, DEFAULT_COLUMN_WIDTH},
{CM_PRINTERLOCATION, TRUE, DEFAULT_COLUMN_WIDTH},
{CM_PRINTERMODEL, TRUE, DEFAULT_COLUMN_WIDTH}};

static const Column_t NETWORK_CONNECTIONS_DEFAULT_COLUMNS[] =
{{CM_NAME, TRUE, DEFAULT_COLUMN_WIDTH},
{CM_TYPE, TRUE, DEFAULT_COLUMN_WIDTH},
{CM_NETWORKADAPTER_STATUS, TRUE, DEFAULT_COLUMN_WIDTH},
{CM_OWNER, TRUE, DEFAULT_COLUMN_WIDTH}};

static const Column_t MY_NETWORK_PLACES_DEFAULT_COLUMNS[] =
{{CM_NAME, TRUE, DEFAULT_COLUMN_WIDTH},
{CM_VIRTUALCOMMENTS, TRUE, DEFAULT_COLUMN_WIDTH}};
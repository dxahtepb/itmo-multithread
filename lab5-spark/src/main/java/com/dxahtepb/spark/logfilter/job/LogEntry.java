package com.dxahtepb.spark.logfilter.job;

import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.time.format.DateTimeParseException;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class LogEntry {
    private static final Pattern PATTERN =
            Pattern.compile("(?<host>.+) - - \\[(?<date>.+)] \"(?<request>.+)\" (?<code>\\d+) (?<bytes>(\\d+|-))");

    private String date;
    private String method;
    private String code;
    private String fullLine;

    private LogEntry(String date, String method, String code, String fullLine) {
        this.date = date;
        this.method = method;
        this.code = code;
        this.fullLine = fullLine;
    }

    public static LogEntry fromLine(String logLine) {
        Matcher matcher = PATTERN.matcher(logLine);
        if (!matcher.matches()) {
            return null;
        }
        String date;
        try {
            date = LocalDateTime
                    .parse(matcher.group("date"), DateTimeFormatter.ofPattern("dd/MMM/yyyy:HH:mm:ss Z"))
                    .toString();
        } catch (DateTimeParseException ex) {
            return null;
        }
        String[] requestSplits = matcher.group("request").split(" ", 2);
        if (requestSplits.length != 2) {
            return null;
        }
        String method = requestSplits[0];
        String code = matcher.group("code");
        return new LogEntry(date, method, code, logLine);
    }

    public String getDate() {
        return date;
    }

    public String getMethod() {
        return method;
    }

    public String getCode() {
        return code;
    }

    public String getFullLine() {
        return fullLine;
    }

    public void setDate(String date) {
        this.date = date;
    }

    public void setMethod(String method) {
        this.method = method;
    }

    public void setCode(String code) {
        this.code = code;
    }

    public void setFullLine(String fullLine) {
        this.fullLine = fullLine;
    }

    @Override
    public String toString() {
        return fullLine;
    }
}

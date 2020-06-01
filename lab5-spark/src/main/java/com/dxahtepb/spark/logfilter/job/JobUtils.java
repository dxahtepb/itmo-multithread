package com.dxahtepb.spark.logfilter.job;

import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.LocatedFileStatus;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.fs.RemoteIterator;
import org.apache.spark.sql.Dataset;
import org.apache.spark.sql.Encoders;
import org.apache.spark.sql.SparkSession;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

final class JobUtils {
    private JobUtils() {
        //empty
    }

    static Dataset<LogEntry> readLog(SparkSession spark, String path) {
        return spark.read()
                .text(path)
                .map(row -> LogEntry.fromLine(row.getString(0)), Encoders.bean(LogEntry.class))
                .filter(Objects::nonNull);
    }

    static void concatResultsHdfs(FileSystem fileSystem, String partsDir, String outputPath) throws IOException {
        RemoteIterator<LocatedFileStatus> files = fileSystem.listFiles(new Path(partsDir), false);
        List<Path> resultFiles = new ArrayList<>();
        while (files.hasNext()) {
            LocatedFileStatus fileStatus = files.next();
            Path path = fileStatus.getPath();
            if (path.getName().startsWith("part-")) {
                if (fileStatus.getLen() == 0) {
                    fileSystem.delete(path, false);
                } else {
                    resultFiles.add(path);
                }
            }
        }
        fileSystem.createNewFile(new Path(outputPath));
        fileSystem.concat(new Path(outputPath), resultFiles.toArray(new Path[0]));
    }
}

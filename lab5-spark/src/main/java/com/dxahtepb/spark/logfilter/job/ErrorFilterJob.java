package com.dxahtepb.spark.logfilter.job;

import org.apache.commons.configuration.Configuration;
import org.apache.hadoop.fs.FSDataOutputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.spark.sql.Dataset;
import org.apache.spark.sql.Row;
import org.apache.spark.sql.SparkSession;

import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;

import static com.dxahtepb.spark.logfilter.job.JobUtils.concatResultsHdfs;
import static com.dxahtepb.spark.logfilter.job.JobUtils.readLog;
import static org.apache.spark.sql.functions.column;

/**
 * Task 2 job
 */
public class ErrorFilterJob implements SparkJob {
    private static final Logger LOG = Logger.getLogger(ErrorFilterJob.class.getName());

    private final SparkSession spark;
    private final FileSystem fileSystem;
    private final String inputLogsPath;
    private final String outputDirPath;
    private final String outputLogsPath;
    private final String outputCountPath;

    private ErrorFilterJob(SparkSession spark, FileSystem fileSystem, String inputLogsPath, String outputDirPath,
                           String outputLogsPath, String outputCountPath) {
        this.spark = spark;
        this.fileSystem = fileSystem;
        this.inputLogsPath = inputLogsPath;
        this.outputDirPath = outputDirPath;
        this.outputLogsPath = outputLogsPath;
        this.outputCountPath = outputCountPath;
    }

    public static SparkJob create(SparkSession spark, FileSystem fileSystem, Configuration conf) {
        String inputLogsPath = conf.getString("input.dir");
        String outputDirPath = conf.getString("output.dir") + "/task_2";
        String outputLogsPath = outputDirPath + "/requests_with_error";
        String outputCountPath = outputDirPath + "/requests_with_error_count";
        return new ErrorFilterJob(spark, fileSystem, inputLogsPath, outputDirPath, outputLogsPath, outputCountPath);
    }

    @Override
    public void run() {
        Dataset<Row> ds = readLog(spark, inputLogsPath)
                .where(column("code").startsWith("5"))
                .sort(column("date"))
                .select(column("fullLine"));
        ds.write().text(outputDirPath);
        long count = ds.count();

        try {
            concatResultsHdfs(fileSystem, outputDirPath, outputLogsPath);
            try (FSDataOutputStream outputStream = fileSystem.create(new Path(outputCountPath))) {
                outputStream.writeBytes(Long.toString(count));
            }
        } catch (IOException e) {
            LOG.log(Level.SEVERE, "HDFS error: ", e);
        }
    }
}

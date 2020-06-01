package com.dxahtepb.spark.logfilter.job;

import org.apache.commons.configuration.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.spark.sql.SparkSession;

import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;

import static com.dxahtepb.spark.logfilter.job.JobUtils.concatResultsHdfs;
import static com.dxahtepb.spark.logfilter.job.JobUtils.readLog;
import static org.apache.spark.sql.functions.column;
import static org.apache.spark.sql.functions.date_format;
import static org.apache.spark.sql.functions.window;

/**
 * Task 4 job
 */
public class ErrorWindowedAggregationJob implements SparkJob {
    private static final Logger LOG = Logger.getLogger(ErrorWindowedAggregationJob.class.getName());

    private final SparkSession spark;
    private final FileSystem fileSystem;
    private final String inputLogsPath;
    private final String outputDirPath;
    private final String outputLogsPath;

    private ErrorWindowedAggregationJob(SparkSession spark, FileSystem fileSystem, String inputLogsPath,
                                       String outputDirPath, String outputLogsPath) {
        this.spark = spark;
        this.fileSystem = fileSystem;
        this.inputLogsPath = inputLogsPath;
        this.outputDirPath = outputDirPath;
        this.outputLogsPath = outputLogsPath;
    }

    public static SparkJob create(SparkSession spark, FileSystem fileSystem, Configuration conf) {
        String inputLogsPath = conf.getString("input.dir");
        String outputDirPath = conf.getString("output.dir") + "/task_4";
        String outputLogsPath = outputDirPath + "/week_aggregation";
        return new ErrorWindowedAggregationJob(spark, fileSystem, inputLogsPath, outputDirPath, outputLogsPath);
    }

    @Override
    public void run() {
        readLog(spark, inputLogsPath)
                .where(column("code").startsWith("5").or(column("code").startsWith("4")))
                .groupBy(window(column("date"), "1 week")).count()
                .sort(column("window"))
                .select(date_format(column("window.start"), "yyyy-MM-dd"),
                        date_format(column("window.end"), "yyyy-MM-dd"),
                        column("count"))
                .write().csv(outputDirPath);

        try {
            concatResultsHdfs(fileSystem, outputDirPath, outputLogsPath);
        } catch (IOException e) {
            LOG.log(Level.SEVERE, "HDFS error: ", e);
        }
    }
}

package com.dxahtepb.spark.logfilter.job;

import org.apache.commons.configuration.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.spark.sql.SparkSession;

import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;

import static com.dxahtepb.spark.logfilter.job.JobUtils.concatResultsHdfs;
import static com.dxahtepb.spark.logfilter.job.JobUtils.readLog;
import static org.apache.spark.sql.functions.*;

/**
 * Task 3 job
 */
public class MethodCodeAggregationJob implements SparkJob {
    private static final Logger LOG = Logger.getLogger(MethodCodeAggregationJob.class.getName());

    private final SparkSession spark;
    private final FileSystem fileSystem;
    private final String inputLogsPath;
    private final String outputDirPath;
    private final String outputLogsPath;

    private MethodCodeAggregationJob(SparkSession spark, FileSystem fileSystem, String inputLogsPath, String outputDirPath,
                             String outputLogsPath) {
        this.spark = spark;
        this.fileSystem = fileSystem;
        this.inputLogsPath = inputLogsPath;
        this.outputDirPath = outputDirPath;
        this.outputLogsPath = outputLogsPath;
    }

    public static SparkJob create(SparkSession spark, FileSystem fileSystem, Configuration conf) {
        String inputLogsPath = conf.getString("input.dir");
        String outputDirPath = conf.getString("output.dir") + "/task_3";
        String outputLogsPath = outputDirPath + "/method_code_day_aggregation";
        return new MethodCodeAggregationJob(spark, fileSystem, inputLogsPath, outputDirPath, outputLogsPath);
    }

    @Override
    public void run() {
        readLog(spark, inputLogsPath)
                .select(date_format(column("date"), "yyyy-MM-dd").as("day"),
                        struct(column("method"), column("code")).as("methodCode"))
                .groupBy(column("methodCode"), column("day")).count()
                .where(column("count").$greater$eq(10))
                .sort(column("methodCode"), column("day"))
                .select(column("methodCode.method"),
                        column("methodCode.code"),
                        column("day"),
                        column("count"))
                .write().csv(outputDirPath);

        try {
            concatResultsHdfs(fileSystem, outputDirPath, outputLogsPath);
        } catch (IOException ex) {
            LOG.log(Level.SEVERE, "HDFS error: ", ex);
        }
    }
}

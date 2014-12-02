
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <getopt.h>
#include <error.h>
#include <string.h>
#include <limits.h>

#include "const.h"
#include "utilities.h"
#include "get_args.h"

/* Default input parameter values */
static float default_wigt = 0.015;
static float default_awgt = 0.0;
static float default_pswt = -0.05;
static float default_percent_slope = 3.0;
static int default_pswnt = 1500;
static int default_pswst = 1000;


/*****************************************************************************
  NAME:  usage

  PURPOSE:  Displays the help/usage to the terminal.

  RETURN VALUE:  None
*****************************************************************************/
void
usage ()
{
    printf ("Dynamic Surface Water Extent\n"
            "Determines and builds surface water extent output bands from"
            " surface\nreflectance input data in ESPA raw binary format.\n\n");
    printf ("usage: dswe"
            " --xml <input_xml_filename> [--help]\n\n");
    printf ("where the following parameters are required:\n");
    printf ("    --xml: name of the input XML file which contains the surface"
            " reflectance,\n"
            "           and top of atmos files output from LEDAPS in raw binary"
            "\n           (envi) format\n");
    printf ("where the following parameters are optional:\n");
    printf ("    --wigt: Modified Normalized Difference Wetness Index"
            " Threshold between 0.00 and 2.00 (default value is %0.3f)\n",
            default_wigt);
    printf ("    --awgt: Automated Water Extent Shadow"
            " Threshold between -2.00 and 2.00"
            " (default value is %0.2f)\n", default_awgt);
    printf ("    --pswt: Partial Surface Water Threshold between -2.00 and"
            " 2.00 (default value is %0.2f)\n", default_pswt);
    printf ("    --pswnt: Partial Surface Water NIR Threshold between 0 and"
            " data maximum (default value is %d)\n", default_pswnt);
    printf ("    --pswst: Partial Surface Water SWIR1 Threshold between 0 and"
            " data maximum (default value is %d)\n", default_pswst);
    printf ("    --percent-slope: Threshold between 0.00 and 100.00"
            " (default value is %0.1f)\n", default_percent_slope);
    printf ("    --use_ledaps_mask: should ledaps cloud/shadow mask be used?"
            " (default is\n"
            "                       false, meaning fmask cloud/shadow will be"
            " used)\n");
    printf ("    --use_zeven_thorne: should Zevenbergen&Thorne's shaded"
            " algorithm be used?\n"
            "                        (default is false, meaning Horn's shaded"
            " algorithm will\n" "                        be used)\n");
    printf ("    --use-toa: should Top of Atmosphere be used instead of"
            " Surface Reflectance\n"
            "               (default is false, meaning Surface Reflectance"
            " will be used)\n");
    printf ("    --verbose: should intermediate messages be printed? (default"
            " is false)\n\n");
    printf ("dswe --help will print this usage statement\n\n");
    printf ("Example: dswe"
            " --xml LE70760172000175AGS00.xml\n");
}

/*****************************************************************************
  NAME:  get_args

  PURPOSE:  Gets the command line arguments and validates that the required
            arguments were specified.

  RETURN VALUE:  Type = int
      Value    Description
      -------  ---------------------------------------------------------------
      ERROR    Error getting the command line arguments or a command line
               argument and associated value were not specified.
      SUCCESS  No errors encountered.
*****************************************************************************/
int
get_args
(
    int argc,                    /* I: number of cmd-line args */
    char *argv[],                /* I: string of cmd-line args */
    char **xml_infile,           /* O: address of input XML filename */
    bool *use_ledaps_mask_flag,  /* O: use ledaps mask */
    bool *use_zeven_thorne_flag, /* O: use zeven thorne */
    bool *use_toa_flag,          /* O: process using TOA */
    float *wigt,                 /* O: tolerance value */
    float *awgt,                 /* O: tolerance value */
    float *pswt,                 /* O: tolerance value */
    float *percent_slope,        /* O: slope tolerance */
    int *pswnt,                  /* O: tolerance value */
    int *pswst,                  /* O: tolerance value */
    bool * verbose_flag          /* O: verbose messaging */
)
{
    int c;
    int option_index;
    char msg[256];
    int tmp_ledaps_mask_flag;
    int tmp_zeven_thorne_flag;
    int tmp_toa_flag;
    int tmp_verbose_flag;

    struct option long_options[] = {
        /* These options set a flag */
        {"use-ledaps-mask", no_argument, &tmp_ledaps_mask_flag, true},
        {"use-zeven-thorne", no_argument, &tmp_zeven_thorne_flag, true},
        {"use-toa", no_argument, &tmp_toa_flag, true},

        /* These options provide values */
        {"xml", required_argument, 0, 'x'},
        {"wigt", required_argument, 0, 'w'},
        {"awgt", required_argument, 0, 'a'},
        {"pswt", required_argument, 0, 'p'},
        {"pswnt", required_argument, 0, 'n'},
        {"pswst", required_argument, 0, 'r'},
        {"percent-slope", required_argument, 0, 's'},

        /* Special options */
        {"verbose", no_argument, &tmp_verbose_flag, true},

        /* The help option */
        {"help", no_argument, 0, 'h'},

        /* The option termination set */
        {0, 0, 0, 0}
    };

    if (argc == 1)
    {
        ERROR_MESSAGE ("Missing required command line arguments\n\n",
                       MODULE_NAME);

        usage ();
        return ERROR;
    }

    /* Assign the default values */
    *wigt = default_wigt;
    *awgt = default_awgt;
    *pswt = default_pswt;
    *percent_slope = default_percent_slope;
    *pswnt = default_pswnt;
    *pswst = default_pswst;

    /* loop through all the cmd-line options */
    opterr = 0; /* turn off getopt_long error msgs as we'll print our own */
    while (1)
    {
        c = getopt_long (argc, argv, "", long_options, &option_index);
        if (c == -1)
        {
            /* out of cmd-line options */
            break;
        }

        switch (c)
        {
        case 0:
            /* If this option set a flag, do nothing else now. */
            if (long_options[option_index].flag != 0)
                break;
        case 'h':
            usage ();
            return ERROR;
            break;
        case 'x':
            *xml_infile = strdup (optarg);
            break;
        case 'w':
            *wigt = atof (optarg);
            break;
        case 'a':
            *awgt = atof (optarg);
            break;
        case 'p':
            *pswt = atof (optarg);
            break;
        case 'n':
            *pswnt = atoi (optarg);
            break;
        case 'r':
            *pswst = atoi (optarg);
            break;
        case 's':
            *percent_slope = atof (optarg);
            break;
        case '?':
        default:
            snprintf (msg, sizeof (msg),
                      "Unknown option %s\n\n", argv[optind - 1]);
            ERROR_MESSAGE (msg, MODULE_NAME);
            usage ();
            return ERROR;
            break;
        }
    }

    /* Grab the boolean command line options */
    if (tmp_ledaps_mask_flag)
        *use_ledaps_mask_flag = true;
    else
        *use_ledaps_mask_flag = false;

    if (tmp_zeven_thorne_flag)
        *use_zeven_thorne_flag = true;
    else
        *use_zeven_thorne_flag = false;

    if (tmp_toa_flag)
        *use_toa_flag = true;
    else
        *use_toa_flag = false;

    if (tmp_verbose_flag)
        *verbose_flag = true;
    else
        *verbose_flag = false;

    /* ---------- Validate the parameters ---------- */
    /* Make sure the XML was specified */
    if (*xml_infile == NULL)
    {
        ERROR_MESSAGE ("XML input file is a required command line"
                       " argument\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    if (*wigt < 0.0 || (*wigt - 2.0) > MINSIGMA)
    {
        ERROR_MESSAGE ("WIGT is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    if ((*awgt + 2) < MINSIGMA || (*awgt - 2.0) > MINSIGMA)
    {
        ERROR_MESSAGE ("AWGT is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    if ((*pswt + 2) < MINSIGMA || (*pswt - 2.0) > MINSIGMA)
    {
        ERROR_MESSAGE ("PSWT is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    /* Only checking the low side here */
    if (*pswnt < 0)
    {
        ERROR_MESSAGE ("PSWB4T is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    /* Only checking the low side here */
    if (*pswst < 0)
    {
        ERROR_MESSAGE ("PSWB4T is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    if (*percent_slope < 0.0 || (*percent_slope - 100.0) > MINSIGMA)
    {
        ERROR_MESSAGE ("Percent Slope is out of range\n\n", MODULE_NAME);

        usage ();
        return ERROR;
    }

    return SUCCESS;
}
